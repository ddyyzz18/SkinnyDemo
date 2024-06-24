#version 330 core


layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

struct DirLight {
    //vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


//uniform vec3 lightPos;
uniform vec3 viewPos;
uniform DirLight light;

const int MAX_BONES = 100;
uniform mat4 finalBonesMatrices[MAX_BONES];
uniform mat4 lightPV;   //シャドウマップ用Lightspaceへ変換

out VS_OUT {
    vec2 TexCoords;
    vec3 TangentLightDir;
    vec3 TangentViewPos;
    vec3 TangentPos;
    vec4 LightSpacePos;//シャドウマップ用Lightspace座標
} vs_out;

void main()
{

    mat4 boneTransform = finalBonesMatrices[boneIds[0]]* weights[0];
    boneTransform += finalBonesMatrices[boneIds[1]]* weights[1];
    boneTransform += finalBonesMatrices[boneIds[2]]* weights[2];
    boneTransform += finalBonesMatrices[boneIds[3]]* weights[3];
    mat4 finalmodel = model * boneTransform;
    mat3 finalnormal = mat3(transpose(inverse(finalmodel))); // animation transform for normal vector

    vec3 PosW = vec3(finalmodel*vec4(pos,1.0));
    vs_out.TexCoords = tex;

    vec3 T = vec3(normalize(finalnormal*tangent));
    vec3 N = vec3(normalize(finalnormal*norm));

    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N)); //TNB is orthogonal:transpose = inverse, but with less cost

    //vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentPos  = TBN * PosW;
    vs_out.TangentLightDir = TBN * light.direction;
    vs_out.LightSpacePos = lightPV*vec4(PosW,1.0f);
    gl_Position =  projection * view * vec4(PosW,1.0);
	
}