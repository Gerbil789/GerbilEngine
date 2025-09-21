#include "enginepch.h"
#include "ShaderSerializer.h"
#include "Engine/Renderer/WebGPUUtils.h"
#include <yaml-cpp/yaml.h>

namespace Engine
{
	static BindingType StringToBindingType(const std::string& str)
	{
		if (str == "uniform-buffer" || str == "uniform")
			return BindingType::UniformBuffer;
		else if (str == "read-only-storage" || str == "storage-buffer")
			return BindingType::StorageBuffer;
		else if (str == "texture-2d" || str == "texture")
			return BindingType::Texture2D;
		else if (str == "sampler")
			return BindingType::Sampler;

		LOG_ERROR("StringToBindingType - Unknown binding type: {0}", str);
		return BindingType::UniformBuffer; // fallback
	}


	ShaderSpecification ShaderSerializer::DeserializeMetadata(const std::string& data, const std::string& shaderName)
	{
		ASSERT(!data.empty(), "Shader metadata is empty!");
		return ShaderSpecification();

		//ShaderSpecification spec;
		//spec.name = shaderName;

		//YAML::Node root = YAML::Load(data);

		//// Vertex Attributes
		//if (root["vertexAttributes"])
		//{
		//	for (const auto& vi : root["vertexAttributes"])
		//	{
		//		ShaderSpecification::VertexAttribute attr;
		//		attr.location = vi["location"].as<uint32_t>();
		//		attr.label = vi["label"].as<std::string>();
		//		attr.format = StringToVertexFormat(vi["format"].as<std::string>());
		//		spec.vertexAttributes.push_back(std::move(attr));
		//	}
		//}

		//// Bind Groups
		//if (root["bindGroups"])
		//{
		//	for (const auto& bg : root["bindGroups"])
		//	{
		//		ShaderSpecification::BindGroup bindGroup;
		//		bindGroup.group = bg["group"].as<uint32_t>();
		//		bindGroup.label = bg["label"].as<std::string>();

		//		if (bg["bindings"])
		//		{
		//			for (const auto& b : bg["bindings"])
		//			{
		//				ShaderSpecification::Binding binding
		//				{
		//					b["binding"].as<uint32_t>(),
		//					b["label"].as<std::string>(),
		//					StringToShaderStage(b["stages"].as<std::string>()),
		//					StringToBindingType(b["type"].as<std::string>()),
		//				};

		//				switch (binding.type)
		//				{
		//				case ShaderSpecification::Binding::Type::UniformBuffer:
		//					binding.bufferType = wgpu::BufferBindingType::Uniform;
		//					break;
		//				case ShaderSpecification::Binding::Type::StorageBuffer:
		//					binding.bufferType = wgpu::BufferBindingType::ReadOnlyStorage;
		//					break;
		//				case ShaderSpecification::Binding::Type::Texture:
		//					binding.textureType = wgpu::TextureSampleType::Float;
		//					break;
		//				case ShaderSpecification::Binding::Type::Sampler:
		//					binding.samplerType = wgpu::SamplerBindingType::Filtering;
		//					break;
		//				}

		//				bindGroup.bindings.push_back(std::move(binding));
		//			}
		//		}

		//		spec.bindGroups.push_back(std::move(bindGroup));
		//	}
		//}

		//if (root["vsEntry"])
		//{
		//	spec.vsEntry = root["vsEntry"].as<std::string>();
		//}

		//if (root["fsEntry"])
		//{
		//	spec.fsEntry = root["fsEntry"].as<std::string>();
		//}

		//return spec;
	}
}