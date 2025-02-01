#version 450

#define MAX_LIGHTS 10

struct LightSource{
	vec3 position;
	vec3 color;
    float intensity;
};



layout(binding=2) uniform sampler2D texSampler;
layout(binding=3) uniform sampler2D specTexSampler;
 
layout(location=0) in vec2 fragTexCoord;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec4 inPos;
layout(location=3) in vec3 camPos;

layout(location=0) out vec4 outColor;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
	vec3 camPos;
	LightSource lightSources[MAX_LIGHTS];
} ubo;

bool valueOutOfRangeOpen(float value, float min, float max);

void main(){
	

    vec3 result=vec3(0.0);
    for(int i=0;i<ubo.lightSources.length();i++){
         // ambient
        float ambientStrength = 0.01;
        vec3 ambient = ambientStrength * ubo.lightSources[i].color*
        texture(texSampler, fragTexCoord).rgb;
  	
        // diffuse 
        vec3 norm = normalize(inNormal);
        vec3 lightDir = normalize(ubo.lightSources[i].position - inPos.xyz);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff* vec3(0.5f, 0.5f, 0.5f)
        * ubo.lightSources[i].color* texture(texSampler, fragTexCoord).rgb
        * ubo.lightSources[i].intensity;
    
        // specular
        float specularStrength = 0.5;
        vec3 viewDir = normalize(camPos - inPos.xyz);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength* vec3(1.0f, 1.0f, 1.0f) * spec
        * ubo.lightSources[i].color*texture(specTexSampler,fragTexCoord).rgb
        * ubo.lightSources[i].intensity;  
        
        result += ambient + diffuse + specular;
    }


   
	outColor=vec4(result,1.0f);
}

bool valueOutOfRangeOpen(float value, float min, float max)
{
    return value<min||value>max;
}