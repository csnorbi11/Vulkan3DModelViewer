#include "ModelLoader.hpp"


ModelLoader::ModelLoader(DeviceManager& deviceManager,
                         VkCommandPool commandPool)
	:
	deviceManager(deviceManager),
	commandPool(commandPool)
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

	size_t index = PATH.find_last_of("\\");
	std::string fileName = PATH.substr(index + 1).c_str();
	std::string path = PATH.substr(0, index + 1).c_str();


	if (!LoadObj(&attrib, &shapes, &materials, &warn, PATH.c_str(), path.c_str()))
	{
		throw std::runtime_error(warn);
	}


	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex{};
			vertex.position = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};
			if (attrib.texcoords.size() != 0)
			{
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
			}

			if (attrib.normals.size() != 0)
			{
				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
			}


			vertices.push_back(vertex);
			indices.push_back(static_cast<uint32_t>(indices.size()));
		}
	}
	bool hasTexture = false;
	for (size_t i = 0; i < materials.size(); i++)
	{
		if (materials[i].diffuse_texname != "")
		{
			textures.emplace_back(deviceManager, commandPool, path + materials[i].diffuse_texname);
		}
	}

	return Model(deviceManager, commandPool, vertices, indices, textures, fileName);
}
