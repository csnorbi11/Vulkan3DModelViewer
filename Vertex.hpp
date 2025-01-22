#pragma once
#include <glm/glm.hpp>
#include "RendererCommon.h"

enum class VertexAttribute : unsigned int
{
	POSITION = 0x01,
	NORMAL = 0x02,
	TEXCOORD = 0x04,
	COLOR = 0x08,
	NONE = 0x00
};

struct VertexColor
{
	glm::vec3 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription
		getBindingDescription();
	static std::vector<VkVertexInputAttributeDescription>
		getAttributeDescriptions();
	bool operator==(const VertexColor& other) const
	{
		return pos == other.pos && color == other.color;
	}
};



struct VertexNormalTexture
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription
		getBindingDescription();
	static std::vector<VkVertexInputAttributeDescription>
		getAttributeDescriptions();
	bool operator==(const VertexNormalTexture& other) const
	{
		return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
	}
};

