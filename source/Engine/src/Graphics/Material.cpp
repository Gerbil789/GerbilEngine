#include "enginepch.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Importer/ShaderImporter.h"
#include "Engine/Core/Resources.h"

namespace Engine
{
	Material::Material(const MaterialSpecification& spec)
	{
		SetShader(spec.shaderId);
	}

	void Material::SetShader(Uuid shaderId)
	{
		m_ShaderId = shaderId;

		m_Parameters.clear();
		m_Textures.clear();

		const Shader& shader = Engine::AssetManager::GetAsset<Shader>(shaderId);

		m_UniformData.assign(shader.GetMaterialUniformBufferSize(), std::byte{});
		CreateUniformBuffer();

		for (const auto& binding : shader.GetMaterialBindings())
		{
			if (std::holds_alternative<BufferBinding>(binding.data))
			{
				const BufferBinding& buffer = std::get<BufferBinding>(binding.data);

				if (buffer.type == wgpu::BufferBindingType::Uniform)
				{
					for (const ShaderParameter& param : buffer.parameters)
					{
						if (param.name[0] == '_') continue; // skip private parameters

						std::visit([&](auto&& value) { SetParameter(param.name, value); }, param.defaultValue);
					}
				}
				else if (buffer.type == wgpu::BufferBindingType::Storage)
				{

				}

			}
			else if (std::holds_alternative<TextureBinding>(binding.data))
			{
				SetTexture(binding.name, Uuid{});
			}
			//else if (std::holds_alternative<SamplerBinding>(binding.data))
			//{
			//	// nothing to do for samplers
			//}
		}

		CreateBindGroup();
	}

	const MaterialValue& Material::GetParameterVariant(const std::string& name) const
	{
		auto it = m_Parameters.find(name);
		if (it != m_Parameters.end())
		{
			return it->second;
		}

		throw std::runtime_error("Parameter not found: " + name);
	}

	void Material::SetTexture(const std::string& name, Uuid texture)
	{
		if (!texture)
		{
			if (name.find("Normal") != std::string::npos)
			{
				texture = RESOURCES::TEXTURE::NORMAL;
			}
			else
			{
				texture = RESOURCES::TEXTURE::WHITE;
			}
		}


		Shader& shader = Engine::AssetManager::GetAsset<Shader>(m_ShaderId);
		const Binding& binding = shader.GetBinding(name);

		if (!std::holds_alternative<TextureBinding>(binding.data))
		{
			LOG_WARNING("Parameter '{}' is not a texture!", name);
			return;
		}

		m_Textures[name] = texture;
		CreateBindGroup(); // recreate bind group to update texture
	}

	Uuid Material::GetTexture(const std::string& name) const
	{
		auto it = m_Textures.find(name);
		if (it != m_Textures.end())
		{
			return it->second;
		}
		return Uuid{};
	}

	void Material::CreateUniformBuffer()
	{
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.label = "MaterialUniformBuffer"; //TODO: add material name
		bufferDesc.size = Engine::AssetManager::GetAsset<Shader>(m_ShaderId).GetMaterialUniformBufferSize(); //TODO: pass size as parameter
		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
		m_UniformBuffer = GraphicsContext::GetDevice().CreateBuffer(&bufferDesc);
	}

	void Material::CreateBindGroup()
	{
		const Shader& shader = Engine::AssetManager::GetAsset<Shader>(m_ShaderId);

		auto materialBindings = shader.GetMaterialBindings();
		size_t bindingCount = std::ranges::distance(materialBindings);

		std::vector<wgpu::BindGroupEntry> entries(bindingCount);

		size_t index = 0;
		for (const auto& binding : materialBindings)
		{
			wgpu::BindGroupEntry& entry = entries[index++];
			entry.binding = binding.binding;

			if (std::holds_alternative<BufferBinding>(binding.data))
			{

				entry.buffer = m_UniformBuffer;
				entry.offset = 0;
				entry.size = m_UniformData.size();
			}
			else if (std::holds_alternative<TextureBinding>(binding.data))
			{
				if (m_Textures.find(binding.name) == m_Textures.end()) //TODO: is this redundant check?
				{
					m_Textures[binding.name] = RESOURCES::TEXTURE::WHITE; 
				}

				const Texture2D& tex = Engine::AssetManager::GetAsset<Texture2D>(m_Textures[binding.name]);
				entry.textureView = tex.GetTextureView();
			}
			else if (std::holds_alternative<SamplerBinding>(binding.data))
			{
				entry.sampler = SamplerPool::GetSampler({ m_TextureFilter, m_TextureWrap });
			}
		}

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.label = "MaterialBindGroup";
		bindGroupDesc.layout = shader.GetMaterialBindGroupLayout();
		bindGroupDesc.entryCount = entries.size();
		bindGroupDesc.entries = entries.data();
		m_BindGroup = GraphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);

		PipelineSpecification spec;
		spec.shaderId = m_ShaderId;

		m_Pipeline = PipelineCache::GetPipeline(spec);
	}


	template<typename T>
	void Material::SetParameter(const std::string& paramName, const T& value)
	{
		Shader& shader = Engine::AssetManager::GetAsset<Shader>(m_ShaderId);
		const Binding& binding = shader.GetBinding("uMaterial");

		if (!std::holds_alternative<BufferBinding>(binding.data))
		{
			LOG_WARNING("Parameter 'uMaterial' is not a buffer binding!");
			return;
		}

		const BufferBinding& buffer = std::get<BufferBinding>(binding.data);

		auto it = std::find_if(buffer.parameters.begin(), buffer.parameters.end(), [&](const ShaderParameter& p) { return p.name == paramName; });

		if (it == buffer.parameters.end())
		{
			LOG_WARNING("Parameter '{}' not found in shader!", paramName);
			return;
		}

		if (sizeof(T) != it->size)
		{
			LOG_WARNING("Size mismatch for '{}'. Expected {}, got {}", paramName, it->size, sizeof(T));
			return;
		}

		std::memcpy(m_UniformData.data() + it->offset, &value, sizeof(T));
		m_Parameters[paramName] = value;
	}

	template ENGINE_API void Material::SetParameter<float>(const std::string&, const float&);
	template ENGINE_API void Material::SetParameter<glm::vec2>(const std::string&, const glm::vec2&);
	template ENGINE_API void Material::SetParameter<glm::vec3>(const std::string&, const glm::vec3&);
	template ENGINE_API void Material::SetParameter<glm::vec4>(const std::string&, const glm::vec4&);
	template ENGINE_API void Material::SetParameter<glm::ivec2>(const std::string&, const glm::ivec2&);
}
