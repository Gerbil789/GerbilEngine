#include "enginepch.h"
#include "MeshImporter.h"
#include "Engine/Core/Project.h"
#include <glm/glm.hpp>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

namespace Engine
{
	Mesh* MeshImporter::ImportMesh(const AssetRecord& metadata)
	{
		return LoadMesh(Project::GetAssetsDirectory() / metadata.path);
	}

	Mesh* MeshImporter::LoadMesh(const std::filesystem::path& path)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err, warn;

		//bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path.string());	// for ASCII glTF(.gltf)
		bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, path.string());		// for binary glTF(.glb)

		if (!warn.empty()) LOG_WARNING("Warning while loading mesh: %s", warn.c_str());
		if (!err.empty()) LOG_ERROR("Error while loading mesh: %s", err.c_str());

		if (!ret)
		{
			LOG_ERROR("Failed to load mesh from file: %s", path);
			return nullptr;
		}

		const tinygltf::Mesh& tinyMesh = model.meshes[0];
		const tinygltf::Primitive& primitive = tinyMesh.primitives[0];

		const auto& attributes = primitive.attributes;

		auto readVec3Array = [&](const std::string& attr) -> std::vector<glm::vec3> {
			std::vector<glm::vec3> result;

			if (attributes.count(attr)) {
				int accessorIndex = attributes.at(attr);
				const auto& accessor = model.accessors[accessorIndex];
				const auto& view = model.bufferViews[accessor.bufferView];
				const auto& buffer = model.buffers[view.buffer];

				const size_t count = accessor.count;
				const size_t stride = accessor.ByteStride(view);
				const uint8_t* data = buffer.data.data() + view.byteOffset + accessor.byteOffset;

				result.reserve(count);
				for (size_t i = 0; i < count; ++i) {
					const float* f = reinterpret_cast<const float*>(data + i * stride);
					result.emplace_back(f[0], f[1], f[2]);
				}
			}

			return result;
			};

		auto readVec2Array = [&](const std::string& attr) -> std::vector<glm::vec2> {
			std::vector<glm::vec2> result;

			if (attributes.count(attr)) {
				int accessorIndex = attributes.at(attr);
				const auto& accessor = model.accessors[accessorIndex];
				const auto& view = model.bufferViews[accessor.bufferView];
				const auto& buffer = model.buffers[view.buffer];

				const size_t count = accessor.count;
				const size_t stride = accessor.ByteStride(view);
				const uint8_t* data = buffer.data.data() + view.byteOffset + accessor.byteOffset;

				result.reserve(count);
				for (size_t i = 0; i < count; ++i) {
					const float* f = reinterpret_cast<const float*>(data + i * stride);
					result.emplace_back(f[0], f[1]);
				}
			}

			return result;
			};

		auto positions = readVec3Array("POSITION");
		auto normals = readVec3Array("NORMAL");
		auto uvs = readVec2Array("TEXCOORD_0");

		const size_t vertexCount = positions.size();
		std::vector<Vertex> vertices;
		vertices.reserve(vertexCount);

		for (size_t i = 0; i < vertexCount; ++i) {
			Vertex v{};
			v.position = positions[i];
			v.normal = (i < normals.size()) ? normals[i] : glm::vec3(0.0f);
			v.uv = (i < uvs.size()) ? uvs[i] : glm::vec2(0.0f);
			vertices.push_back(v);
		}

		// === Index data ===
		std::vector<uint16_t> indices;

		if (primitive.indices >= 0) {
			const auto& accessor = model.accessors[primitive.indices];
			const auto& view = model.bufferViews[accessor.bufferView];
			const auto& buffer = model.buffers[view.buffer];

			const uint8_t* data = buffer.data.data() + view.byteOffset + accessor.byteOffset;
			const size_t count = accessor.count;
			indices.reserve(count);

			switch (accessor.componentType) {
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
				const uint16_t* src = reinterpret_cast<const uint16_t*>(data);
				indices.assign(src, src + count);
				break;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
				const uint8_t* src = reinterpret_cast<const uint8_t*>(data);
				for (size_t i = 0; i < count; ++i)
					indices.push_back(static_cast<uint16_t>(src[i]));
				break;
			}
			default:
				LOG_ERROR("Unsupported index format.");
				break;
			}
		}

		std::vector<uint16_t> wireIndices;
		wireIndices.reserve(indices.size() * 2); // 3 edges = 6 indices per triangle

		for (size_t i = 0; i + 2 < indices.size(); i += 3)
		{
			uint16_t i0 = indices[i + 0];
			uint16_t i1 = indices[i + 1];
			uint16_t i2 = indices[i + 2];

			wireIndices.push_back(i0); wireIndices.push_back(i1);
			wireIndices.push_back(i1); wireIndices.push_back(i2);
			wireIndices.push_back(i2); wireIndices.push_back(i0);
		}

		MeshSpecification spec{ vertices, indices, wireIndices }; //TODO: is this copy or reference?
		Mesh* mesh = new Mesh(spec);
		return mesh;
	}
}