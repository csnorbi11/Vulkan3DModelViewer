#include "ModelLoader.hpp"




ModelLoader::~ModelLoader()
{
}

ModelLoader::ModelLoader(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
	const VkCommandPool& commandPool, const VkQueue& queue,
	const VkPhysicalDeviceProperties& properties)
	:
	device(device),
	physicalDevice(physicalDevice),
	commandPool(commandPool),
	queue(queue),
	properties(properties)
{
}

Model ModelLoader::loadModel(const std::string PATH, bool verticalFlipTexture)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Texture> textures;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, PATH.c_str())) {
		throw std::runtime_error(warn);
	}

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			vertex.position = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};
			if (verticalFlipTexture)
				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};
			else
				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};

			vertices.push_back(vertex);
			indices.push_back(indices.size());
		}
	}

	for (size_t i = 0; i < materials.size(); i++) {
		textures.emplace_back(device, physicalDevice, commandPool, queue, materials[i].diffuse_texname,properties);
	}
	size_t index = PATH.find_last_of("\\");
	std::string name = PATH.substr(index + 1).c_str();
	return Model(device, physicalDevice, commandPool, queue, vertices, indices, textures, name);
}

