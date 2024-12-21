#version 450

layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(binding=1) uniform ModelUniformBufferObject{
	mat4 model;
} mubo;

void main(){
	gl_Position = ubo.proj * ubo.view * mubo.model * vec4(inPosition, 1.0);
	//gl_Position = vec4(inPosition, 1.0);
	fragTexCoord=inTexCoord;
}