#version 400

const int MAX_LIGHTS =14;

layout(location=0) in vec4 VertexPos;
layout(location=1) in vec4 VertexNormal;
layout(location=2) in vec2 VertexTexcoord;
layout(location=3) in vec3 VertexTangente;
layout(location=4) in vec3 VertexBitangente;

out vec3 Position;
out vec3 Normal;
out vec2 Texcoord;
out vec3 Tangente;
out vec3 Bitangente; 
out vec4 ShadowCoord[MAX_LIGHTS];

uniform mat4 ModelMat;
uniform mat4 ModelViewProjMat;
uniform mat4 ShadowMapMats[MAX_LIGHTS];

void main()
{
    
    Position    = (ModelMat * VertexPos).xyz;
    Normal      = (ModelMat * vec4(VertexNormal.xyz,0)).xyz;
    Texcoord    = VertexTexcoord;
    //gl_Position = ModelViewProjMat * VertexPos;
    Tangente    = (ModelMat * vec4(VertexTangente, 0)).xyz;
    Bitangente  = (ModelMat * vec4(VertexBitangente, 0)).xyz;

    // Pro Licht Shadow-Koordinaten berechnen
    for(int i = 0; i < MAX_LIGHTS; ++i) {
        ShadowCoord[i] = ShadowMapMats[i] * ModelMat *VertexPos;
    }

    gl_Position = ModelViewProjMat * VertexPos;

}

