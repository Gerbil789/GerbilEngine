#include "enginepch.h"
#include "Engine/Asset/Importer/MeshImporter.h"
#include "Engine/Core/Engine.h"
#include "Engine/Graphics/Material.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

namespace Engine
{
	static glm::mat4 GetNodeMatrix(const tinygltf::Node& node)
	{
		if (node.matrix.size() == 16)
		{
			glm::mat4 m(1.0f);
			for (int i = 0; i < 16; ++i)
				m[i / 4][i % 4] = static_cast<float>(node.matrix[i]);
			return m;
		}

		glm::vec3 t(0.0f);
		glm::quat r(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 s(1.0f);

		if (node.translation.size() == 3)
		{
			t = glm::vec3(
				static_cast<float>(node.translation[0]),
				static_cast<float>(node.translation[1]),
				static_cast<float>(node.translation[2]));
		}

		if (node.rotation.size() == 4)
		{
			// glTF order: x, y, z, w
			r = glm::quat(
				static_cast<float>(node.rotation[3]),
				static_cast<float>(node.rotation[0]),
				static_cast<float>(node.rotation[1]),
				static_cast<float>(node.rotation[2]));
		}

		if (node.scale.size() == 3)
		{
			s = glm::vec3(
				static_cast<float>(node.scale[0]),
				static_cast<float>(node.scale[1]),
				static_cast<float>(node.scale[2]));
		}

		return glm::translate(glm::mat4(1.0f), t)
			* glm::mat4_cast(r)
			* glm::scale(glm::mat4(1.0f), s);
	}

	static void ReadVec3(
		const tinygltf::Model& model,
		int accessorIndex,
		std::vector<glm::vec3>& out)
	{
		const auto& accessor = model.accessors[accessorIndex];
		const auto& view = model.bufferViews[accessor.bufferView];
		const auto& buffer = model.buffers[view.buffer];

		size_t stride = accessor.ByteStride(view);
		if (stride == 0) stride = sizeof(float) * 3;

		const uint8_t* data = buffer.data.data() + view.byteOffset + accessor.byteOffset;

		out.resize(accessor.count);
		for (size_t i = 0; i < accessor.count; ++i)
		{
			const float* f = reinterpret_cast<const float*>(data + i * stride);
			out[i] = { f[0], f[1], f[2] };
		}
	}

	static void ReadVec2(
		const tinygltf::Model& model,
		int accessorIndex,
		std::vector<glm::vec2>& out)
	{
		const auto& accessor = model.accessors[accessorIndex];
		const auto& view = model.bufferViews[accessor.bufferView];
		const auto& buffer = model.buffers[view.buffer];

		size_t stride = accessor.ByteStride(view);
		if (stride == 0) stride = sizeof(float) * 2;

		const uint8_t* data = buffer.data.data() + view.byteOffset + accessor.byteOffset;

		out.resize(accessor.count);
		for (size_t i = 0; i < accessor.count; ++i)
		{
			const float* f = reinterpret_cast<const float*>(data + i * stride);
			out[i] = { f[0], f[1] };
		}
	}

	static void ProcessNode(
		const tinygltf::Model& model, int nodeIndex, const glm::mat4& parentTransform,
		std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, std::vector<uint32_t>& wireIndices, 
		std::vector<SubMesh>& subMeshes)
	{
		const auto& node = model.nodes[nodeIndex];
		glm::mat4 transform = parentTransform * GetNodeMatrix(node);
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));

		if (node.mesh >= 0)
		{
			const auto& mesh = model.meshes[node.mesh];

			for (const auto& primitive : mesh.primitives)
			{
				const uint32_t vertexOffset = static_cast<uint32_t>(vertices.size());   // local to this primitive
				const uint32_t firstIndex = static_cast<uint32_t>(indices.size());      // global index buffer start

				std::vector<glm::vec3> positions;
				std::vector<glm::vec3> normals;
				std::vector<glm::vec2> uvs;

				if (primitive.attributes.count("POSITION"))
					ReadVec3(model, primitive.attributes.at("POSITION"), positions);

				if (primitive.attributes.count("NORMAL"))
					ReadVec3(model, primitive.attributes.at("NORMAL"), normals);

				if (primitive.attributes.count("TEXCOORD_0"))
					ReadVec2(model, primitive.attributes.at("TEXCOORD_0"), uvs);

				for (size_t i = 0; i < positions.size(); ++i)
				{
					Vertex v{};
					v.position = glm::vec3(transform * glm::vec4(positions[i], 1.0f));
					v.normal = i < normals.size()
						? glm::normalize(normalMatrix * normals[i])
						: glm::vec3(0.0f);
					v.uv = i < uvs.size() ? uvs[i] : glm::vec2(0.0f);
					vertices.push_back(v);
				}


				if (primitive.indices >= 0)
				{
					const auto& accessor = model.accessors[primitive.indices];
					const auto& view = model.bufferViews[accessor.bufferView];
					const auto& buffer = model.buffers[view.buffer];

					const uint8_t* data = buffer.data.data() + view.byteOffset + accessor.byteOffset;

					switch (accessor.componentType)
					{
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
					{
						const uint8_t* src = (const uint8_t*)data;
						for (size_t i = 0; i < accessor.count; ++i)
							indices.push_back(vertexOffset + src[i]);
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
					{
						const uint16_t* src = (const uint16_t*)data;
						for (size_t i = 0; i < accessor.count; ++i)
							indices.push_back(vertexOffset + src[i]);
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
					{
						const uint32_t* src = (const uint32_t*)data;
						for (size_t i = 0; i < accessor.count; ++i)
							indices.push_back(vertexOffset + src[i]);
						break;
					}
					default:
						LOG_ERROR("Unsupported index type");
						break;
					}
					uint32_t materialIndex = primitive.material >= 0? primitive.material : 0; // fallback

					SubMesh sub{};
					sub.firstIndex = firstIndex;
					sub.indexCount = static_cast<uint32_t>(accessor.count);
					sub.materialIndex = materialIndex;
					subMeshes.push_back(sub);

					for (size_t i = 0; i < accessor.count; i += 3)
					{
						uint32_t i0 = indices[firstIndex + i + 0];
						uint32_t i1 = indices[firstIndex + i + 1];
						uint32_t i2 = indices[firstIndex + i + 2];

						wireIndices.push_back(i0); wireIndices.push_back(i1);
						wireIndices.push_back(i1); wireIndices.push_back(i2);
						wireIndices.push_back(i2); wireIndices.push_back(i0);
					}

				}
			}
		}

		for (int child : node.children)
		{
			ProcessNode(model, child, transform, vertices, indices, wireIndices, subMeshes);
		}

	}

	Mesh* MeshImporter::ImportMesh(const AssetRecord& metadata)
	{
		return LoadMesh(Engine::GetAssetsDirectory() / metadata.path);
	}

	Mesh* MeshImporter::LoadMesh(const std::filesystem::path& path)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err, warn;

		if (!loader.LoadBinaryFromFile(&model, &err, &warn, path.string()))
		{
			LOG_ERROR("Failed to load glTF: {}", path);
			return nullptr;
		}

		if (!warn.empty()) LOG_WARNING("{}", warn);
		if (!err.empty()) LOG_ERROR("{}", err);

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<uint32_t> wireIndices;

		const tinygltf::Scene& scene = model.scenes[model.defaultScene >= 0 ? model.defaultScene : 0];

		std::vector<SubMesh> subMeshes;

		for (int node : scene.nodes)
		{
			ProcessNode(model, node, glm::mat4(1.0f), vertices, indices, wireIndices, subMeshes);
		}

		std::vector<std::string> materialNames;
		for (auto mat : model.materials)
		{
			materialNames.push_back(mat.name);
		}

		return new Mesh({ vertices, indices, wireIndices, subMeshes, materialNames });
	}
}