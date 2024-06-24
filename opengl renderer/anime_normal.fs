#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
    //vec3 PosW;
    vec3 TangentLightDir;
    //vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentPos;
    vec4 LightSpacePos;//シャドウマップ用Lightspace座標
} fs_in;

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

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform sampler2D shadowMap;    //シャドウマップ用テクスチャ

//uniform vec3 lightPos;
uniform vec3 viewPos;
uniform DirLight light;

vec3 CalcDirLight(vec3 normal);
//vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float CalShadowMap(vec4,vec3);
void main()
{           
     // obtain normal from normal map in range [0,1]
    vec3 normal = texture(texture_normal1, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

    FragColor = vec4(CalcDirLight(normal), 1.0);

}

vec3 CalcDirLight(vec3 normal)
{
    vec3 lightDir = normalize(-fs_in.TangentLightDir); 
    // diffuse shading
    float diff = max(dot(lightDir,normal), 0.0);
    // specular shading
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentPos);
    vec3 reflectDir = reflect(lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // shadow 
    float shadow = CalShadowMap(fs_in.LightSpacePos,normal);
    // combine results
    vec3 ambient = light.ambient * texture(texture_diffuse1, fs_in.TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, fs_in.TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(texture_specular1, fs_in.TexCoords).rgb;
    return (ambient + (diffuse + specular)*(1.0-shadow));
    
}
float CalShadowMap(vec4 pos,vec3 normal)
{
    // perform perspective divide
    vec3 projCoords = pos.xyz / pos.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    //vec3 normal = normalize(fs_in.Normal);
    //vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, normalize(light.direction))), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}