#version 450

#define MAX_LIGHTS 32

struct LightSource{
	vec3 position;
	vec3 color;
};

layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location=1) out vec3 outNormal;
layout(location=2) out vec4 fragPos;
layout(location=3) out vec3 camPos;
layout(location=4) out LightSource outLightSources[MAX_LIGHTS];

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
	vec3 camPos;
	LightSource lightSources[MAX_LIGHTS];
} ubo;

layout(binding=1) uniform ModelUniformBufferObject{
	mat4 model;
} mubo;

void main(){

	fragTexCoord=inTexCoord;
	outNormal=mat3(transpose(inverse(mubo.model))) * inNormal;
	fragPos= vec4(mubo.model * vec4(inPosition, 1.0));
	camPos=ubo.camPos;
	outLightSources=ubo.lightSources;

	gl_Position = ubo.proj * ubo.view * fragPos;
}