#version 450

#define MAX_LIGHTS 32

struct LightSource{
	vec3 position;
	vec3 color;
};

layout(binding=2) uniform sampler2D texSampler;
 
layout(location=0) in vec2 fragTexCoord;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec4 inPos;
layout(location=3) in vec3 camPos;
layout(location=4) in LightSource lightSources[MAX_LIGHTS];

layout(location=0) out vec4 outColor;


void main(){
	

    vec3 result;
    for(int i=0;i<lightSources.length();i++){
         // ambient
        float ambientStrength = 0.01;
        vec3 ambient = ambientStrength * lightSources[i].color;
  	
        // diffuse 
        vec3 norm = normalize(inNormal);
        vec3 lightDir = normalize(lightSources[i].position - inPos.xyz);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightSources[i].color;
    
        // specular
        float specularStrength = 0.5;
        vec3 viewDir = normalize(camPos - inPos.xyz);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * lightSources[i].color;  
        
        result += (ambient + diffuse + specular) * texture(texSampler, fragTexCoord).rgb;
    }


   
	outColor=vec4(result,1.0f);
}