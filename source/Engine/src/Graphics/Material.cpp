#include "enginepch.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Importer/ShaderImporter.h"
#include "Engine/Core/Log.h"

namespace engine
{
	static wgpu::Buffer CreateUniformBuffer(std::string_view name, size_t size)
	{
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.label = name;
		bufferDesc.size = size;
		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
		return GraphicsContext::GetDevice().CreateBuffer(&bufferDesc);
	}

	MaterialAsset::MaterialAsset(Shader shader)
	{
		this->shader = shader;

		const ShaderAsset& shaderData = shader.Get();		defaultUniformData.assign(shaderData.materialUniformBufferSize, std::byte{});
		defaultUniformData.assign(shaderData.materialUniformBufferSize, std::byte{});

		for (const auto& binding : shader.GetMaterialBindings())
		{
			if (std::holds_alternative<BufferBinding>(binding.data))
			{
				const BufferBinding& buffer = std::get<BufferBinding>(binding.data);

				if (buffer.type == wgpu::BufferBindingType::Uniform)
				{
					for (const ShaderParameter& param : buffer.parameters)
					{
						if (param.name[0] == '_') continue;
//		std::variant<float, glm::vec2, glm::vec3, glm::vec4, glm::ivec2> defaultValue;

						if(std::holds_alternative<float>(param.defaultValue))
						{
							auto value = std::get<float>(param.defaultValue);
							std::memcpy(defaultUniformData.data() + param.offset, &value, sizeof(float));
						}


					}
				}
				else if (buffer.type == wgpu::BufferBindingType::Storage)
				{
				}
			}
			// else if (std::holds_alternative<TextureBinding>(binding.data))
			// {
			// 	SetTexture(binding.binding, {});
			// }
		}

		PipelineSpecification spec;
		spec.shader = shader;
		pipeline = PipelineCache::GetPipeline(spec);
	}

	MaterialAsset& Material::Get() const
	{
		return AssetManager::GetAsset(*this);
	}

	Shader Material::GetShader() const
	{
		return Get().shader;
	}

	wgpu::RenderPipeline Material::GetPipeline() const
	{
		return Get().pipeline;
	}

	size_t Material::GetExpectedSize() const 
	{ 
		// LOG_TRACE("Expected size: {}", Get().defaultUniformData.size());
		return Get().defaultUniformData.size(); 
	}

	MaterialInstanceData::MaterialInstanceData(Material material)
	{
		this->material = material;
		uniformBuffer = CreateUniformBuffer("MaterialUniformBuffer", material.GetExpectedSize());
	}

	void MaterialInstanceData::CreateBindGroup()
	{
		const MaterialAsset& materialData = material.Get();
		const ShaderAsset& shaderData = materialData.shader.Get();

		auto materialBindings = materialData.shader.GetMaterialBindings();
		size_t bindingCount = std::ranges::distance(materialBindings);

		std::vector<wgpu::BindGroupEntry> entries(bindingCount);

		size_t index = 0;
		for (const auto& binding : materialBindings)
		{
			wgpu::BindGroupEntry& entry = entries[index++];
			entry.binding = binding.binding;

			if (std::holds_alternative<BufferBinding>(binding.data))
			{
				entry.buffer = uniformBuffer;
				entry.offset = 0;
				entry.size = uniformData.size();
			}
			else if (std::holds_alternative<TextureBinding>(binding.data))
			{
				// if (m_Textures.find(binding.binding) == m_Textures.end()) // TODO: is this redundant check?
				// {
				 	textures[binding.binding] = Texture2D::White;
				// }

				const Texture2D& texture = textures.at(binding.binding);
				entry.textureView = texture.View();
			}
			else if (std::holds_alternative<SamplerBinding>(binding.data))
			{
				entry.sampler = SamplerPool::GetSampler({ textureFilter, textureWrap });
			}
		}

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.label = "MaterialBindGroup";
		bindGroupDesc.layout = shaderData.materialBindGroupLayout;
		bindGroupDesc.entryCount = entries.size();
		bindGroupDesc.entries = entries.data();
		bindGroup = GraphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);
	}



	MaterialInstanceData& MaterialInstance::Get() const
	{
		return AssetManager::GetAsset(*this);
	}

	const Shader MaterialInstance::GetShader() const
	{
		return Get().material.GetShader();
	}

	void MaterialInstance::SetTextureFilter(TextureFilter filter)
	{
		MaterialInstanceData& data = Get();
		data.textureFilter = filter;
		data.CreateBindGroup();
	}

	void MaterialInstance::SetTextureWrap(TextureWrap wrap)
	{
		MaterialInstanceData& data = Get();
		data.textureWrap = wrap;
		data.CreateBindGroup();
	}

	TextureFilter MaterialInstance::GetTextureFilter() const
	{
		return Get().textureFilter;
	}

	TextureWrap MaterialInstance::GetTextureWrap() const
	{
		return Get().textureWrap;
	}

	void MaterialInstance::SetTexture(uint32_t bindingIndex, Texture2D texture)
	{
		MaterialInstanceData& data = Get();
		const Binding& binding = data.material.GetShader().GetBinding(2, bindingIndex);

		if (!std::holds_alternative<TextureBinding>(binding.data))
		{
			LOG_WARNING("Parameter '{}' is not a texture!", binding.name);
			return;
		}

		if (!texture)
		{
			if (binding.name.find("Normal") != std::string::npos)
			{
				texture = Texture2D::Normal;
			}
			else
			{
				texture = Texture2D::White;
			}
		}

		data.textures[bindingIndex] = texture;
		data.CreateBindGroup();
	}


	Texture2D MaterialInstance::GetTexture(uint32_t bindingIndex) const
	{
		auto& textures = Get().textures;
		auto it = textures.find(bindingIndex);
		if (it != textures.end())
		{
			return it->second;
		}
		return {};
	}

	wgpu::BindGroup MaterialInstance::GetBindGroup() const
	{
		return Get().bindGroup;
	}

	wgpu::Buffer MaterialInstance::GetUniformBuffer() const
	{
		return Get().uniformBuffer;
	}

	const std::vector<std::byte>& MaterialInstance::GetUniformData() const
	{
		return Get().uniformData;
	}

	const std::unordered_map<uint32_t, Texture2D>& MaterialInstance::GetTextures() const
	{
		return Get().textures;
	}

	// const std::unordered_map<std::string, MaterialValue>& Material::GetParameters() const
	// {
	// 	return Get().parameters;
	// }



	// template<typename T>
	// void Material::SetParameter(const std::string& name, const T& value)
	// {
	// 	MaterialAsset& data = Get();
	// 	const Binding& binding = data.shader.GetBinding("uMaterial");

	// 	if (!std::holds_alternative<BufferBinding>(binding.data))
	// 	{
	// 		LOG_WARNING("Parameter 'uMaterial' is not a buffer binding!");
	// 		return;
	// 	}

	// 	const BufferBinding& buffer = std::get<BufferBinding>(binding.data);

	// 	auto it = std::find_if(buffer.parameters.begin(), buffer.parameters.end(), [&](const ShaderParameter& p)
	// 		{ return p.name == name; });

	// 	if (it == buffer.parameters.end())
	// 	{
	// 		LOG_WARNING("Parameter '{}' not found in shader!", name);
	// 		return;
	// 	}

	// 	if (sizeof(T) != it->size)
	// 	{
	// 		LOG_WARNING("Size mismatch for '{}'. Expected {}, got {}", name, it->size, sizeof(T));
	// 		return;
	// 	}

	// 	std::memcpy(data.uniformData.data() + it->offset, &value, sizeof(T));
	// 	data.parameters[name] = value;
	// }

	// template void Material::SetParameter<float>(const std::string&, const float&);
	// template void Material::SetParameter<glm::vec2>(const std::string&, const glm::vec2&);
	// template void Material::SetParameter<glm::vec3>(const std::string&, const glm::vec3&);
	// template void Material::SetParameter<glm::vec4>(const std::string&, const glm::vec4&);
	// template void Material::SetParameter<glm::ivec2>(const std::string&, const glm::ivec2&);

	// const MaterialValue& Material::GetParameterVariant(const std::string& name) const
	// {
	// 	auto& parameters = Get().parameters;

	// 	auto it = parameters.find(name);
	// 	if (it != parameters.end())
	// 	{
	// 		return it->second;
	// 	}

	// 	throw std::runtime_error("Parameter not found: " + name);
	// }
}