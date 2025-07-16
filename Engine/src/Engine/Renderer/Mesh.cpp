#include "enginepch.h"
#include "Mesh.h"
#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Events/WindowEvent.h"
#include "Engine/Core/Application.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tiny_gltf.h>

namespace Engine
{
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
	};

	Ref<Asset> MeshFactory::Load(const std::filesystem::path& path, const std::any& data)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err, warn;

		//bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path.string());	// for ASCII glTF(.gltf)
		bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, path.string());		// for binary glTF(.glb)

		if (!warn.empty()) LOG_WARNING("MeshFactory", "Warning while loading mesh: %s", warn.c_str());
		if (!err.empty()) LOG_ERROR("MeshFactory", "Error while loading mesh: %s", err.c_str());

		if (!ret)
		{
			LOG_ERROR("MeshFactory", "Failed to load mesh from file: %s", path);
			return nullptr;
		}

		const tinygltf::Mesh& mesh = model.meshes[0];
		const tinygltf::Primitive& primitive = mesh.primitives[0];

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
					indices.push_back(static_cast<uint32_t>(src[i]));
				break;
			}
			default:
				LOG_ERROR("MeshFactory", "Unsupported index format.");
				break;
			}
		}

		// debug log
		for(int i = 0; i < vertices.size(); i++)
		{
			auto vertex = vertices[i];
			LOG_TRACE("Vertex {0}: [{1},{2},{3}]", i, vertex.position.x, vertex.position.y, vertex.position.z);
		}

		for (int i = 0; i < indices.size(); i++)
		{
			auto index = indices[i];
			LOG_TRACE("Index {0}: [{1}]", i, index);
		}


		// === Upload to GPU ===

		auto device = GraphicsContext::GetDevice(); 

		// Vertex buffer
		wgpu::BufferDescriptor vertexBufferdesc{};
		vertexBufferdesc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
		vertexBufferdesc.size = vertices.size() * sizeof(Vertex);
		vertexBufferdesc.mappedAtCreation = false;

		wgpu::Buffer vertexBuffer = device.createBuffer(vertexBufferdesc);
		GraphicsContext::GetQueue().writeBuffer(vertexBuffer, 0, vertices.data(), vertexBufferdesc.size);

		// Index buffer
		wgpu::BufferDescriptor indexBufferdesc{};
		indexBufferdesc.usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
		indexBufferdesc.size = indices.size() * sizeof(uint16_t);
		indexBufferdesc.mappedAtCreation = false;

		wgpu::Buffer indexBuffer = device.createBuffer(indexBufferdesc);
		GraphicsContext::GetQueue().writeBuffer(indexBuffer, 0, indices.data(), indexBufferdesc.size);

		return CreateRef<Mesh>(path, vertexBuffer, indexBuffer, indices.size());
	}
}