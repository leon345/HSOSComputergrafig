#version 400

const int MAX_LIGHTS =14;

uniform vec3 EyePos;

uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;

uniform sampler2D MixTex;
uniform sampler2D DetailTex[4];
uniform sampler2D NormalTex[4];
uniform sampler2D ShadowMapTexture[MAX_LIGHTS];
uniform mat4 ShadowMapMat[MAX_LIGHTS];

struct Light {
    int Type;             // 0=Point, 1=Directional, 2=Spot
    vec3 Color;
    vec3 Position;
    vec3 Direction;
    vec3 Attenuation;
    vec2 SpotRadius;      // x = inner, y = outer (in radians)
    int ShadowIndex;      // -1 = kein Shadow
};

uniform Lights {
    int LightCount;
    Light lights[MAX_LIGHTS];
};

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;
in vec3 Tangente;
in vec3 Bitangente;

out vec4 FragColor;

float sat(float a) { return clamp(a, 0.0, 1.0); }

mat3 createTBN(vec3 T, vec3 B, vec3 N) {
    return mat3(normalize(T), normalize(B), normalize(N));
}

float QuantizeValue(float f, float steps){
    f = floor(f*(steps+1));
    f/= steps;
    return f;
}

float CalcOutline(vec3 N, vec3 E, float degree){
    float Outline = abs( dot(N, E));

    if(Outline < cos(3.14 * degree/180.0)){
        Outline = 0.9;
    } else{
        Outline = 1;
    }
    return Outline;
}

float calculateShadow(int shadowIndex, vec3 worldPos, vec3 N, vec3 L) {
    if(shadowIndex < 0) return 1.0; // kein Schatten

    vec4 lightSpacePos = ShadowMapMat[shadowIndex] * vec4(worldPos, 1.0);
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Frustum-Grenzen für bessere Schatten-Begrenzung
    if (projCoords.z > 0.99 || projCoords.x < 0.01 || projCoords.x > 0.99 || 
        projCoords.y < 0.01 || projCoords.y > 0.99)
        return 1.0; // Kein Schatten außerhalb des Frustums

    // Adaptive Bias basierend auf Normale und Lichtrichtung
    //vec3 lightDir = normalize(-lights[shadowIndex].Direction);
    float bias = max(0.01, 0.05 * (1.0 - abs(dot(N, L))));
    vec2 texelSize = 1.0 / textureSize(ShadowMapTexture[shadowIndex], 0);
    float shadow = 0.0;
    int sampleRadius = 1; // Größerer Kernel für weichere Schatten

    // PCF für weichere Kanten
    for (int x = -sampleRadius; x <= sampleRadius; ++x) {
        for (int y = -sampleRadius; y <= sampleRadius; ++y) {
            vec2 offset = vec2(x, y) * texelSize;
            float pcfDepth = texture(ShadowMapTexture[shadowIndex], projCoords.xy + offset).r;
            shadow += projCoords.z - bias > pcfDepth ? 0.0 : 1.0;
        }
    }
    shadow /= 9.0; // 5x5 = 25 samples
    
    // Soft-Falloff am Rand des Shadow-Maps
    vec2 edgeFactor = smoothstep(0.0, 0.1, projCoords.xy) * 
                      (1.0 - smoothstep(0.9, 1.0, projCoords.xy));
    float edgeBlend = edgeFactor.x * edgeFactor.y;
    
    return mix(1.0, shadow, edgeBlend);
}

vec3 calculatePhongShading(vec3 L, vec3 E, vec3 N, vec3 lightColor, vec4 DiffTex) {
    vec3 H = normalize(L + E);
    float diff = sat(dot(N, L));
    float spec = pow(max(dot(N, H), 0.0), SpecularExp);

    vec3 diffuse  = lightColor * DiffuseColor * diff;
    vec3 specular = lightColor * SpecularColor * spec;

    return (diffuse * DiffTex.rgb + specular);
}
//Toon Shader Vorlesung 10
vec3 calculateToonShading(vec3 L, vec3 E, vec3 N, vec3 lightColor, vec4 DiffTex){
    vec3 R = reflect(-L, N);
    float DiffQ = QuantizeValue(sat(dot(N, L)), 50);
    vec3 Diff = lightColor * DiffuseColor * DiffTex.rgb * DiffQ; //TODO eigentlich TextColor.rgb

    float SpecQ = QuantizeValue(pow(sat(dot(R, E)), SpecularExp), 50);
    vec3 Spec = lightColor * SpecularColor * SpecQ;

    vec3 Amb = AmbientColor * DiffTex.rgb;

    float Outline = CalcOutline(N, E, 70.0);

    return Outline * (Spec + Diff + Amb);
}

vec3 calculatePointLight(Light light, vec3 pos, vec3 N, vec3 E, vec4 DiffTex) {
    vec3 L = normalize(light.Position - pos);
    float dist = length(light.Position - pos);
    float attenuation = 1.0 / (light.Attenuation.x + light.Attenuation.y * dist + light.Attenuation.z * dist * dist);

    float shadow = calculateShadow(light.ShadowIndex, pos, N, L);
    //return calculatePhongShading(L, N, E, light.Color * attenuation, DiffTex) * shadow;
    return calculateToonShading(L, N, E, light.Color * attenuation, DiffTex) * shadow;
}

vec3 calculateDirectionalLight(Light light, vec3 pos, vec3 N, vec3 E, vec4 DiffTex) {
    vec3 L = normalize(-light.Direction);
    float shadow = calculateShadow(light.ShadowIndex, pos, N, L);
    //return calculatePhongShading(L, N, E, light.Color, DiffTex) * shadow;
    return calculateToonShading(L, N, E, light.Color, DiffTex) * shadow;
}

vec3 calculateSpotLight(Light light, vec3 pos, vec3 N, vec3 E, vec4 DiffTex) {
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
    //return calculatePhongShading(L, N, E, light.Color * attenuation * spotEffect, DiffTex) * shadow;
    return calculateToonShading(L, N, E, light.Color * attenuation * spotEffect, DiffTex) * shadow;
}


// sichere Normalmap-Sample-Funktion
vec3 sampleNormalSafe(sampler2D tex, vec2 uv) {
    vec3 n = texture(tex, uv).rgb;
    // Wenn Textur leer (0,0,0) oder default (0.5,0.5,1.0) -> fallback
    if (length(n) < 0.001) 
        return vec3(0.0, 0.0, 1.0); // Default normal (tangent space Z)

    return normalize(n * 2.0 - 1.0);
}

// besseres Normal-Blending
vec3 blendNormals(vec3 n1, vec3 n2, float factor) {
    n1 = normalize(n1);
    n2 = normalize(n2);
    return normalize(mix(n1, n2, factor));
}



void main()
{
    vec3 D = EyePos - Position;
    float Dist = length(D);
    vec3 E = D / Dist;

    // --- Fog setup ---
    float dMin = 0.0, dMax = 50.0, a = 1.5;
    float fogFactor = clamp(pow((Dist - dMin) / (dMax - dMin), a), 0, 1);
    vec4 cFog = vec4(0.25, 0.35, 0.4, 1.0);

    // --- Texturen ---
    vec4 mixMap = texture(MixTex, Texcoord);
    vec4 grassTex = texture(DetailTex[0], Texcoord * 100.0);
    vec4 rockTex  = texture(DetailTex[1], Texcoord * 100.0);
    vec4 sandTex  = texture(DetailTex[2], Texcoord * 100.0);
    vec4 snowTex  = texture(DetailTex[3], Texcoord * 100.0);

    // --- Normalmaps ---
    vec3 grassNormal = sampleNormalSafe(NormalTex[0], Texcoord * 100.0);
    vec3 rockNormal  = sampleNormalSafe(NormalTex[1], Texcoord * 100.0);
    vec3 sandNormal  = sampleNormalSafe(NormalTex[2], Texcoord * 100.0);
    vec3 snowNormal  = sampleNormalSafe(NormalTex[3], Texcoord * 100.0);

    // --- Höhenabhängiges Blending ---
    float weichTief = smoothstep(0.0, 3.0, Position.y); 
    float weichHoch = smoothstep(11.0, 16.0, Position.y);  // Direkter Höhenwert für bessere Kontrolle

    vec4 interpolatedColor = mix(grassTex, rockTex, mixMap.r);
    interpolatedColor = mix(sandTex, interpolatedColor, weichTief);
    interpolatedColor = mix(interpolatedColor, snowTex, weichHoch);

    // --- Normal Blending ---
    vec3 blendedNormal    = blendNormals(grassNormal, rockNormal, mixMap.r);
    vec3 sandGrassNormal  = blendNormals(sandNormal, blendedNormal, weichTief);
    vec3 finalNormal      = blendNormals(sandGrassNormal, snowNormal, weichHoch);

    mat3 TBN = createTBN(Tangente, Bitangente, Normal);
    vec3 worldNormal = normalize(TBN * finalNormal);

    // --- Licht Berechnung ---
    vec3 totalLighting = AmbientColor * interpolatedColor.rgb;

    for (int i = 0; i < LightCount && i < MAX_LIGHTS; ++i) {
        Light light = lights[i];
        
        if(light.Type == 0) {
            // Point Light
            totalLighting += calculatePointLight(light, Position, worldNormal, E, interpolatedColor);
        }
        else if(light.Type == 1) {
            // Directional Light
            totalLighting += calculateDirectionalLight(light, Position, worldNormal, E, interpolatedColor);
        }
        else if(light.Type == 2) {
            // Spot Light
            totalLighting += calculateSpotLight(light, Position, worldNormal, E, interpolatedColor);
        }
    }

    vec4 cOrg = vec4(totalLighting, interpolatedColor.a);


    // --- Fog mix ---
    FragColor = mix(cOrg, cFog, fogFactor);
    //FragColor = vec4(worldNormal, 1);
    //FragColor = toonColor;


}