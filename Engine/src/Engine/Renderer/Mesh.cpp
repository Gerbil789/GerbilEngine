#include "enginepch.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Events/WindowEvent.h"
#include "Engine/Core/Application.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.

#include <tiny_gltf.h>

namespace Engine
{
	Ref<Asset> MeshFactory::Load(const std::filesystem::path& path, const std::any& data)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		//bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path.string());
		bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, path.string()); // for binary glTF(.glb)

		if (!warn.empty()) {
			printf("Warn: %s\n", warn.c_str());
		}

		if (!err.empty()) {
			printf("Err: %s\n", err.c_str());
		}

		if (!ret) {
			printf("Failed to parse glTF\n");
			return nullptr;
		}

		const tinygltf::Mesh& mesh = model.meshes[0];
		const tinygltf::Primitive& primitive = mesh.primitives[0];

		// === VERTEX POSITIONS ===
		int posAccessorIndex = primitive.attributes.at("POSITION");
		const tinygltf::Accessor& posAccessor = model.accessors[posAccessorIndex];
		const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
		const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];

		const size_t vertexCount = posAccessor.count;
		const size_t stride = tinygltf::GetComponentSizeInBytes(posAccessor.componentType) * tinygltf::GetNumComponentsInType(posAccessor.type);

		std::vector<glm::vec3> positions;
		positions.reserve(vertexCount);

		const unsigned char* posData = posBuffer.data.data() + posView.byteOffset + posAccessor.byteOffset;

		for (size_t i = 0; i < vertexCount; ++i) {
			float* pos = (float*)(posData + i * stride);
			positions.emplace_back(pos[0], pos[1], pos[2]);
		}


		std::vector<uint32_t> indices;

		if (primitive.indices >= 0) {
			const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
			const tinygltf::BufferView& indexView = model.bufferViews[indexAccessor.bufferView];
			const tinygltf::Buffer& indexBuffer = model.buffers[indexView.buffer];

			const unsigned char* indexData = indexBuffer.data.data() + indexView.byteOffset + indexAccessor.byteOffset;
			const size_t indexCount = indexAccessor.count;

			indices.reserve(indexCount);

			switch (indexAccessor.componentType) {
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
				const uint16_t* src = reinterpret_cast<const uint16_t*>(indexData);
				for (size_t i = 0; i < indexCount; ++i)
					indices.push_back(static_cast<uint32_t>(src[i]));
				break;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
				const uint32_t* src = reinterpret_cast<const uint32_t*>(indexData);
				indices.assign(src, src + indexCount);
				break;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
				const uint8_t* src = reinterpret_cast<const uint8_t*>(indexData);
				for (size_t i = 0; i < indexCount; ++i)
					indices.push_back(static_cast<uint32_t>(src[i]));
				break;
			}
			default:
				// Unsupported index type
				break;
			}
		}


		return CreateRef<Mesh>(path);
	}
}