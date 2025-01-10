#version 450

layout(binding=0) in inColor;

layout(binding=0)out outColor;


void main(){
	outColor=inColor;
}