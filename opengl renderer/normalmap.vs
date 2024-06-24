#version 330 core

#define GS // Gram-Schmidt re-orthogonalize 

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 normal;

uniform vec3 lightPos;
uniform vec3 viewPos;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

out VS_OUT {
    vec2 TexCoords;
    vec3 PosW;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentWorldPos;
} vs_out;

void main()
{

    mat4 boneTransform = finalBonesMatrices[boneIds[0]]* weights[0];
    boneTransform += finalBonesMatrices[boneIds[1]]* weights[1];
    boneTransform += finalBonesMatrices[boneIds[2]]* weights[2];
    boneTransform += finalBonesMatrices[boneIds[3]]* weights[3];
    mat4 boneNormalTrans = transpose(inverse(boneTransform)); // animation transform for normal vector

    vec4 PosL = boneTransform*vec4(pos,1.0);
    vs_out.PosW = vec3(model*PosL);
    vec3 NormalL = vec3(boneNormalTrans* vec4(norm,1.0));
    vec3 TangentL = vec3(boneTransform* vec4(tangent,1.0));
    vs_out.TexCoords = tex;
    vec3 T = vec3(normalize(normal*vec4(TangentL,1.0)));
    vec3 N = vec3(normalize(normal*vec4(NormalL,1.0)));

#ifdef  GS
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
#endif
    vec3 B = cross(N, T);
#ifdef GS
    mat3 TBN = transpose(mat3(T, B, N)); //TNB is orthogonal:transpose = inverse, but with less cost
#else
    mat3 TBN = inverse(mat3(T, B, N));
#endif
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentWorldPos  = TBN * vs_out.PosW;

    gl_Position =  projection * view * vec4(vs_out.PosW,1.0);
	
}