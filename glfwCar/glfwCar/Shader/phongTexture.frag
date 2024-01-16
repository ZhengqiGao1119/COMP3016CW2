#version 450 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;


struct PointLight {
    vec3 lightColor;
    vec3 lightPos;
    vec3 lightAmbient;
    vec3 lightDiffuse;
    vec3 lightSpecular;
    float lightConstant;
    float lightLinear;
    float lightQuadratic;
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
    bool lightOn;
};


struct Material {
    vec3 matAmbient;
    vec3 matDiffuse;
    vec3 matSpecular;
    float matShininess;
};

uniform bool useTexture;
uniform vec3 viewPos;  
uniform PointLight pointLights[1];
uniform SpotLight spotLight[1];

uniform vec3 carPos;
uniform Material material;
uniform sampler2D texture1;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 color;
    if(useTexture)
        color = texture(texture1, TexCoord).rgb;
    else 
        color = material.matDiffuse;
    
    vec3 lightDir = normalize(light.lightPos - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.matShininess);
    
    // attenuation
    float distance = length(light.lightPos - fragPos);
    float attenuation = 1.0 / (light.lightConstant + light.lightLinear * distance + light.lightQuadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.lightAmbient * color;
    vec3 diffuse  = light.lightColor * light.lightDiffuse * diff * color;
    vec3 specular = light.lightColor * light.lightSpecular * spec * color;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 color;
    if(useTexture)
        color = texture(texture1, TexCoord).rgb;
    else 
        color = material.matDiffuse;

    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.matShininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient  = light.ambient * color;
    vec3 diffuse  = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * color;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

void main() {
    vec4 texColor = texture(texture1, TexCoord);

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.f);
    for(int i = 0; i < 1; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    }

    for(int i = 0; i < 1; i++) {
        if(spotLight[i].lightOn) {
            vec3 viewDirCar = normalize(carPos - FragPos); 
            result += CalcSpotLight(spotLight[i], norm, FragPos, viewDirCar);    
        }
    }

    FragColor = vec4(result, 1.f);
}
