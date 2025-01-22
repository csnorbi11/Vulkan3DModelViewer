#include "Shader.hpp"






Shader::~Shader()
{
	vkDestroyShaderModule(device, shaderModule, nullptr);
}

Shader::Shader(VkDevice device, const std::string& filename, ShaderType type,
	VertexAttribute vertexAttributes)
	:
	device(device),
	vertexAttributes(vertexAttributes),
	type(type)
{
	createModule(device, filename);

}

void Shader::createModule(VkDevice device, const std::string& filename)
{
	auto code = readFile(filename);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}
}

VkShaderModule Shader::getModule() const
{
	return shaderModule;
}
VertexAttribute Shader::getVertexAttributes() const
{
	return vertexAttributes;
}
ShaderType Shader::getType() const
{
	return type;
}
