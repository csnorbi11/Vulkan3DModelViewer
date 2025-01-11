#version 450


layout(location=0) in vec3 inPosition;
layout(location=3) in vec3 inColor;

layout(location = 0) out vec3 outColor;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(binding=1) uniform ModelUniformBufferObject{
	mat4 model;
} mubo;

void main(){
	outColor=inColor;
	gl_Position = ubo.proj * ubo.view * mubo.model * vec4(inPosition,1.0f);
}