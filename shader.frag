#version 450

layout(binding=2) uniform sampler2D texSampler;
 
layout(location=0) in vec2 fragTexCoord;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec4 inPos;
layout(location=3) in vec3 camPos;

layout(location=0) out vec4 outColor;

vec3 lightPos=vec3(0.0,2.0,0.0);
vec3 lightColor=vec3(1.0,0.2,0.2);

void main(){
	
    // ambient
    float ambientStrength = 0.01;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(inNormal);
    vec3 lightDir = normalize(lightPos - inPos.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(camPos - inPos.xyz);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * texture(texSampler, fragTexCoord).rgb;
	outColor=vec4(result,1.0f);
}