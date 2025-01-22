#pragma once
#include "RendererCommon.h"
#include "Vertex.hpp"

enum class ShaderType
{
	VERTEX,
	FRAGMENT
};

class Shader
{
public:
	Shader() = default;
	~Shader();

	Shader(VkDevice device, const std::string& filename, ShaderType type,
		VertexAttribute vertexAttributes = VertexAttribute::NONE);

	VkShaderModule getModule() const;
	VertexAttribute getVertexAttributes() const;
	ShaderType getType() const;


private:
	void createModule(VkDevice device, const std::string& filename);

	VkShaderModule shaderModule;
	VertexAttribute vertexAttributes;
	ShaderType type;

	VkDevice device;
};



constexpr VertexAttribute operator|(VertexAttribute lhs, VertexAttribute rhs) {
	return static_cast<VertexAttribute>(
		static_cast<std::underlying_type_t<VertexAttribute>>(lhs) |
		static_cast<std::underlying_type_t<VertexAttribute>>(rhs));
}

constexpr VertexAttribute operator&(VertexAttribute lhs, VertexAttribute rhs) {
	return static_cast<VertexAttribute>(
		static_cast<std::underlying_type_t<VertexAttribute>>(lhs) &
		static_cast<std::underlying_type_t<VertexAttribute>>(rhs));
}