#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
    //vec3 PosW;
    vec3 TangentLightDir;
    //vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentPos;
} fs_in;

struct Light {
    //vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

//uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Light light;

void main()
{           
     // obtain normal from normal map in range [0,1]
    vec3 normal = texture(texture_normal1, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
   
    // ambient
    vec3 ambient = light.ambient * texture(texture_diffuse1, fs_in.TexCoords).rgb;
    // diffuse 
    // vec3 lightDir = normalize(light.position - FragPos);
    vec3 lightDir = normalize(-fs_in.TangentLightDir);  
    float diff = max(dot(lightDir,normal), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, fs_in.TexCoords).rgb;  

    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentPos);
    vec3 reflectDir = reflect(lightDir, normal);  
    //vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * spec * texture(texture_specular1, fs_in.TexCoords).rgb;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
    //FragColor = vec4(normal, 1.0);
    //FragColor = vec4(texture(texture_specular1, fs_in.TexCoords).rgb,1.0);
}