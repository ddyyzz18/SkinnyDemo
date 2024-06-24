#version 330 core
out vec4 FragColor;


in VS_OUT {
    vec2 TexCoords;
    vec3 PosW;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentWorldPos;
} fs_in;

struct Light {
    vec3 position;  
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform Light light;
//uniform vec3 viewPos;

void main()
{    
    // obtain normal from normal map in range [0,1]
    vec3 normal = texture(texture_normal1, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

    // ambient
    vec3 ambient = light.ambient * texture(texture_diffuse1, fs_in.TexCoords).rgb;
  
    // diffuse 
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentWorldPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, fs_in.TexCoords).rgb;  
    
    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentWorldPos);
    //vec3 reflectDir = reflect(-lightDir, normal);   //phong
    vec3 halfwayDir = normalize(lightDir + viewDir);  //blinn-phong
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = light.specular * spec * texture(texture_specular1, fs_in.TexCoords).rgb;  
    
    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff); 
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;
    
    // attenuation
    float distance    = length(light.position - fs_in.PosW);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;   
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
      //FragColor = texture(texture_diffuse1, TexCoords);
}