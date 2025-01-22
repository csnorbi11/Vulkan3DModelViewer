#pragma once
#include "RendererCommon.h"

class Shader
{
public:
	Shader() = default;
	~Shader();

	Shader(VkDevice device, const std::string& filename,
		VertexAttribute vertexAttributes);

	VkShaderModule getModule() const;
	const VkVertexInputBindingDescription& getBindingDescription() const;
	const std::vector<VkVertexInputAttributeDescription>& getAttributeDescriptions() const;

	void cleanup(VkDevice device);

private:
	void createModule(VkDevice device, const std::string& filename);

	VkShaderModule shaderModule;
	VkVertexInputBindingDescription bindingDescription;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

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