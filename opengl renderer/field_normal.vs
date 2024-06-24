#version 330 core


layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;


struct DirLight {
    //vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform mat4 projection;
uniform mat4 view;
uniform mat3 model;


//uniform vec3 lightPos;
uniform vec3 viewPos;
uniform DirLight light;
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

    //mat3 normal = mat3(transpose(inverse(model))); // animation transform for normal vector

    vec3 PosW = vec3(model*pos);
    vs_out.TexCoords = tex;

    vec3 T = vec3(normalize(model*tangent));
    vec3 N = vec3(normalize(model*norm));

    // re-orthogonalize T with respect to N
    // T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N)); //TNB is orthogonal:transpose = inverse, but with less cost

    //vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentPos  = TBN * PosW;
    vs_out.TangentLightDir = TBN * light.direction;
    vs_out.LightSpacePos = lightPV*vec4(PosW,1.0f);
    gl_Position =  projection * view * vec4(PosW,1.0);
	
}