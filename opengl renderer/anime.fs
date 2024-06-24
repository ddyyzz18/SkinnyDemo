#version 330 core
out vec4 FragColor;

in vec3 wPos;  
in vec3 wNormal;  
in vec2 TexCoords;

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
uniform Light light;
uniform vec3 viewPos;

void main()
{    
  // ambient
  vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb;
  
  // diffuse 
  vec3 norm = normalize(wNormal);
  vec3 lightDir = normalize(light.position - wPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;  
  
  // specular
  vec3 viewDir = normalize(viewPos - wPos);
  vec3 reflectDir = reflect(-lightDir, norm);  
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
  vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;  
  
  // spotlight (soft edges)
  float theta = dot(lightDir, normalize(-light.direction)); 
  float epsilon = (light.cutOff - light.outerCutOff); 
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
  diffuse  *= intensity;
  specular *= intensity;
  
  // attenuation
  float distance    = length(light.position - wPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
  ambient  *= attenuation; 
  diffuse   *= attenuation;
  specular *= attenuation;   
      
  vec3 result = ambient + diffuse + specular;
  FragColor = vec4(result, 1.0);
    //FragColor = texture(texture_diffuse1, TexCoords);
}