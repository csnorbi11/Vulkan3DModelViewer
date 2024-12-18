#include "ModelLoader.hpp"




ModelLoader::~ModelLoader()
{
	cleanup();
}

ModelLoader::ModelLoader(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
	const VkCommandPool& commandPool, const VkQueue& queue,
	std::shared_ptr<std::vector<Model>> models)
	:
	device(device),
	physicalDevice(physicalDevice),
	commandPool(commandPool),
	queue(queue),
	models(models)
{
}

void ModelLoader::loadModel(const std::string PATH)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, PATH.c_str())) {
		throw std::runtime_error(err);
	}

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			vertex.position = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

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

	models->emplace_back(device, physicalDevice, commandPool, queue, vertices, indices);
}

void ModelLoader::cleanup()
{
	for (auto& model : *models)
	{
		model.cleanup();
	}
}
