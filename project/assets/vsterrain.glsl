#version 400

layout(location=0) in vec4 VertexPos;
layout(location=1) in vec4 VertexNormal;
layout(location=2) in vec2 VertexTexcoord;
layout(location=3) in vec2 VertexTexcoord1;
layout(location=4) in vec3 VertexTangente;
layout(location=5) in vec3 VertexBitangente;

out vec3 Position;
out vec3 Normal;
out vec2 Texcoord;
out vec2 Texcoord1;
out vec3 Tangente;
out vec3 Bitangente;

uniform mat4 ModelMat;
uniform mat4 ModelViewProjMat;
uniform vec3 Scaling;

void main()
{
    vec4 scaledPos = vec4(VertexPos.x * Scaling.x,
                          VertexPos.y * Scaling.y,
                          VertexPos.z * Scaling.z,
                          1.0);

    vec4 NewVertexNormal = vec4(VertexNormal.x / Scaling.x,
                                VertexNormal.y / Scaling.y,
                                VertexNormal.z / Scaling.z,
                                1.0);
    

    Position = (ModelMat * scaledPos).xyz;
    Normal = (ModelMat * vec4(NewVertexNormal.xyz,0)).xyz;
    Texcoord = VertexTexcoord;
    Texcoord1 = VertexTexcoord1;

    Tangente = (ModelMat * vec4(VertexTangente, 0)).xyz;
    //Bitangente = (ModelMat * vec4(VertexBitangente, 0)).xyz;

    //optimierte Version Gram-Shmidt Quelle: Lawonn, Kai. „Computer Graphics II  - Normal Mapping“. Uni Jena, o. J. Zugegriffen 28. August 2025. https://vis.uni-jena.de/Lecture/ComputerGraphics2/Lec10_b_NormalMapping.pdf.
    Tangente = normalize(Tangente - dot(Tangente, Normal) * Normal);
    Bitangente = cross(Normal, Tangente);
    gl_Position = ModelViewProjMat * scaledPos;
}
