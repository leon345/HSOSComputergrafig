#version 400

const int MAX_LIGHTS = 14;

// Eingaben aus Vertexshader
in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;
in vec3 Tangente;
in vec3 Bitangente;
in vec4 ShadowCoord[MAX_LIGHTS];

struct Light {
    int Type;             // 0=Point, 1=Directional, 2=Spot
    vec3 Color;
    vec3 Position;
    vec3 Direction;
    vec3 Attenuation;
    vec2 SpotRadius;      // x = inner, y = outer
    int ShadowIndex;      // -1 = kein Shadow
};

out vec4 FragColor;

uniform vec3 EyePos;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;

uniform sampler2D DiffuseTexture;
uniform sampler2D NormalTexture;

uniform sampler2D ShadowMapTexture[MAX_LIGHTS];
uniform mat4 ShadowMapMat[MAX_LIGHTS];

uniform Lights {
    int LightCount;
    Light lights[MAX_LIGHTS];
};

//outline
uniform vec3 OutlineColor;
uniform float OutlineWidth;

vec3 calculateOutline(vec3 e, vec3 n, vec3 color, float width){
    float cosAngle = 1- abs(dot(n, e));
    return color * pow(cosAngle, width);
}

float sat(float a) { return clamp(a, 0.0, 1.0); }

// Schattenberechnung mit PCF
float calculateShadow(int shadowIndex, vec3 worldPos, vec3 N, vec3 lightDir) {
    if(shadowIndex < 0) return 1.0;

    vec4 lightSpacePos = ShadowMapMat[shadowIndex] * vec4(worldPos, 1.0);
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 0.99 || projCoords.x < 0.01 || projCoords.x > 0.99 ||
       projCoords.y < 0.01 || projCoords.y > 0.99)
        return 1.0;

    float bias = max(0.01, 0.05 * (1.0 - abs(dot(N, lightDir))));
    vec2 texelSize = 1.0 / textureSize(ShadowMapTexture[shadowIndex], 0);

    float shadow = 0.0;
    int sampleRadius = 1;

    for(int x = -sampleRadius; x <= sampleRadius; ++x) {
        for(int y = -sampleRadius; y <= sampleRadius; ++y) {
            vec2 offset = vec2(x, y) * texelSize;
            float pcfDepth = texture(ShadowMapTexture[shadowIndex], projCoords.xy + offset).r;
            shadow += projCoords.z - bias > pcfDepth ? 0.0 : 1.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

vec3 calculatePhongShading(vec3 L, vec3 N, vec3 E, vec3 lightColor, vec4 DiffTex) {
    vec3 H = normalize(L + E);
    float diff = sat(dot(N, L));
    float spec = pow(max(dot(N, H), 0.0), SpecularExp);

    vec3 diffuse  = lightColor * DiffuseColor * diff;
    vec3 specular = lightColor * SpecularColor * spec;

    return (diffuse * DiffTex.rgb + specular);
}

vec3 calculatePointLight(Light light, vec3 pos, vec3 E, vec3 N, vec4 DiffTex) {
    vec3 L = normalize(light.Position - pos);
    float dist = length(light.Position - pos);
    float attenuation = 1.0 / (light.Attenuation.x + light.Attenuation.y * dist + light.Attenuation.z * dist * dist);

    float shadow = calculateShadow(light.ShadowIndex, pos, N, L);
    return calculatePhongShading(L, N, E, light.Color * attenuation, DiffTex) * shadow;
}

vec3 calculateDirectionalLight(Light light, vec3 pos, vec3 E, vec3 N, vec4 DiffTex) {
    vec3 L = normalize(-light.Direction);
    float shadow = calculateShadow(light.ShadowIndex, pos, N, L);
    return calculatePhongShading(L, N, E, light.Color, DiffTex) * shadow;
}

vec3 calculateSpotLight(Light light, vec3 pos, vec3 E, vec3 N, vec4 DiffTex) {
    vec3 L = light.Position - pos;
    float dist = length(L);
    L = normalize(L);

    float attenuation = 1.0 / (light.Attenuation.x + light.Attenuation.y * dist + light.Attenuation.z * dist * dist);

    float cosAngle = dot(-L, normalize(light.Direction));
    float phiInner = cos(light.SpotRadius.x);
    float phiOuter = cos(light.SpotRadius.y);
    if(cosAngle <= phiOuter) return vec3(0.0);

    float spotEffect = smoothstep(phiOuter, phiInner, cosAngle);

    float shadow = calculateShadow(light.ShadowIndex, pos, N, L);
    return calculatePhongShading(L, N, E, light.Color * attenuation * spotEffect, DiffTex) * shadow;
}

mat3 createTBN(vec3 tangente, vec3 bitangente, vec3 normal) {
    vec3 T = normalize(tangente);
    vec3 B = normalize(bitangente);
    vec3 N = normalize(normal);
    return mat3(T, B, N);
}

void main()
{
    vec4 DiffTex = texture(DiffuseTexture, Texcoord);
    if(DiffTex.a < 0.3) discard;

    // Normalmapping
    vec3 normalMap = texture(NormalTexture, Texcoord).rgb * 2.0 - 1.0;
    mat3 TBN = createTBN(Tangente, Bitangente, Normal);
    vec3 N = normalize(TBN * normalMap);

    vec3 E = normalize(EyePos - Position);
    vec3 totalColor = AmbientColor * DiffTex.rgb;

    for(int i = 0; i < LightCount && i < MAX_LIGHTS; ++i) {
        Light light = lights[i];
        if(light.Type == 0)
            totalColor += calculatePointLight(light, Position, E, N, DiffTex);
        else if(light.Type == 1)
            totalColor += calculateDirectionalLight(light, Position, E, N, DiffTex);
        else if(light.Type == 2)
            totalColor += calculateSpotLight(light, Position, E, N, DiffTex);
    }
 
    if(OutlineWidth != 0.0){
        vec3 outline = calculateOutline(E, N, OutlineColor, OutlineWidth);
        totalColor += outline;
    }
    FragColor = vec4(totalColor.rgb, DiffTex.a);

}
