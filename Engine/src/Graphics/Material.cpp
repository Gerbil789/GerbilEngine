#include "enginepch.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Importer/ShaderImporter.h"
#include "Engine/Core/Resources.h"

namespace Engine
{
	static Material* s_DefaultMaterial = nullptr;

	template<typename T>
	void Material::SetParameter(const std::string& paramName, const T& value)
	{
		auto binding = m_Shader.GetBinding("uMaterial");

		if (binding.type != BindingType::UniformBuffer)
		{
			LOG_WARNING("Parameter 'uMaterial' is not a uniform buffer!");
			return;
		}

		auto it = std::find_if(binding.parameters.begin(), binding.parameters.end(),
			[&](const ShaderParameter& p) { return p.name == paramName; });

		if (it == binding.parameters.end())
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

	const MaterialValue& Material::GetParameterVariant(const std::string& name) const
	{
		auto it = m_Parameters.find(name);
		if (it != m_Parameters.end())
			return it->second;

		throw std::runtime_error("Parameter not found: " + name);
	}

	Material::Material(const MaterialSpecification& spec)
	{
		m_Shader = Engine::AssetManager::GetAsset<Shader>(spec.shaderId);
		m_TextureFilter = spec.filter;
		m_TextureWrap = spec.wrap;

		m_PipelineSpec.shaderId = spec.shaderId;

		m_UniformData.resize(m_Shader.GetMaterialUniformBufferSize(), std::byte{});
		CreateUniformBuffer();


		for (const auto& [name, variantValue] : spec.parameters)
		{
			std::visit([&](auto&& arg) 
				{
				SetParameter(name, arg);
				}, variantValue);
		}

		for (auto& [name, uuid] : spec.textures)
		{
			SetTexture(name, uuid);
		}

		// material bindings
		for (const auto& binding : m_Shader.GetMaterialBindings())
		{
			if (binding.type == BindingType::Texture2D)
			{
				auto it = spec.textures.find(binding.name);
				if (it == spec.textures.end())
				{
					SetTexture(binding.name, RESOURCES::INVALID_UUID);
				}
			}
		}

		CreateBindGroup();
	}

	void Material::SetShader(Uuid shaderId)
	{
		m_Shader = Engine::AssetManager::GetAsset<Shader>(shaderId);

		m_UniformData.resize(m_Shader.GetMaterialUniformBufferSize(), std::byte{});
		CreateUniformBuffer();
		CreateBindGroup();
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

		auto binding = m_Shader.GetBinding(name);

		if (binding.type != BindingType::Texture2D)
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
		return RESOURCES::INVALID_UUID;
	}

	void Material::CreateUniformBuffer()
	{
		wgpu::BufferDescriptor bufferDesc{};
		bufferDesc.label = { "MaterialUniformBuffer", WGPU_STRLEN }; //TODO: add material name
		bufferDesc.size = m_Shader.GetMaterialUniformBufferSize();
		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
		m_UniformBuffer = GraphicsContext::GetDevice().createBuffer(bufferDesc);
	}

	void Material::CreateBindGroup()
	{
		auto materialBindings = m_Shader.GetMaterialBindings();
		size_t bindingCount = std::ranges::distance(materialBindings);

		std::vector<wgpu::BindGroupEntry> entries(bindingCount);

		size_t index = 0;
		for (const auto& binding : materialBindings)
		{
			wgpu::BindGroupEntry& entry = entries[index++];
			entry.binding = binding.binding;

			if (binding.type == BindingType::UniformBuffer)
			{
				entry.buffer = m_UniformBuffer;
				entry.offset = 0;
				entry.size = m_UniformData.size();
			}
			else if (binding.type == BindingType::Texture2D)
			{
				if (m_Textures.find(binding.name) == m_Textures.end()) //TODO: is this redundant check?
				{
					m_Textures[binding.name] = RESOURCES::TEXTURE::WHITE; 
				}

				const Texture& tex = Engine::AssetManager::GetAsset<Texture2D>(m_Textures[binding.name]);
				entry.textureView = tex.GetTextureView();
			}
			else if (binding.type == BindingType::Sampler)
			{
				entry.sampler = SamplerPool::GetSampler(m_TextureFilter, m_TextureWrap);
			}
		}

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.label = { "MaterialBindGroup", WGPU_STRLEN };
		bindGroupDesc.layout = m_Shader.GetMaterialBindGroupLayout();
		bindGroupDesc.entryCount = entries.size();
		bindGroupDesc.entries = entries.data();
		m_BindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
	}

	template ENGINE_API void Material::SetParameter<float>(const std::string&, const float&);
	template ENGINE_API void Material::SetParameter<glm::vec2>(const std::string&, const glm::vec2&);
	template ENGINE_API void Material::SetParameter<glm::vec3>(const std::string&, const glm::vec3&);
	template ENGINE_API void Material::SetParameter<glm::vec4>(const std::string&, const glm::vec4&);
}
