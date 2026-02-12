#include "enginepch.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Importer/ShaderImporter.h"

namespace Engine
{
	static Material* s_DefaultMaterial = nullptr;

	Material::Material(const MaterialSpecification& spec)
	{
		m_Shader = spec.shader;
		m_TextureFilter = spec.filter;
		m_TextureWrap = spec.wrap;

		m_UniformData.resize(m_Shader->GetMaterialUniformBufferSize(), std::byte{});
		CreateUniformBuffer();

		for (auto& [name, value] : spec.floatDefaults)
			SetFloat(name, value);

		for (auto& [name, value] : spec.vec4Defaults)
			SetVec4(name, value);

		for (auto& [name, uuid] : spec.textureDefaults)
		{
			if (auto tex = AssetManager::GetAsset<Texture2D>(uuid))
				SetTexture(name, tex);
		}

		CreateBindGroup();
	}

	void Material::SetShader(Shader* shader)
	{
		m_Shader = shader;

		m_UniformData.resize(m_Shader->GetMaterialUniformBufferSize(), std::byte{});
		CreateUniformBuffer();
		CreateBindGroup();
	}

	void Material::SetFloat(const std::string& paramName, float value)
	{
		auto binding = GetBinding(GetMaterialBindings(m_Shader->GetSpecification()), "uMaterialUniforms");
		if (binding.type != BindingType::UniformBuffer)
		{
			LOG_WARNING("Material::SetFloat - Parameter 'uMaterialUniforms' is not a uniform buffer!");
			return;
		}

		auto it = std::find_if(binding.parameters.begin(), binding.parameters.end(), [&](const ShaderParameter& p) { return p.name == paramName; });

		if (it == binding.parameters.end() || it->type != ShaderValueType::Float)
		{
			LOG_WARNING("Material::SetFloat - Parameter '{}' not found or not a float!", paramName);
			return;
		}

		std::memcpy(m_UniformData.data() + it->offset, &value, sizeof(float));
	}

	void Material::SetVec4(const std::string& paramName, const glm::vec4& value)
	{
		auto materialBindings = GetMaterialBindings(m_Shader->GetSpecification());
		auto binding = GetBinding(materialBindings, "uMaterialUniforms");
		if (binding.type != BindingType::UniformBuffer)
		{
			LOG_WARNING("Material::SetFloat - Parameter '{}' is not a uniform buffer!", "uMaterialUniforms");
			return;
		}

		auto it = std::find_if(binding.parameters.begin(), binding.parameters.end(), [&](const ShaderParameter& p) { return p.name == paramName; });

		if (it == binding.parameters.end() || it->type != ShaderValueType::Vec4)
		{
			LOG_WARNING("Material::SetFloat - Parameter '{}' not found or not a float!", paramName);
			return;
		}

		std::memcpy(m_UniformData.data() + it->offset, &value, sizeof(glm::vec4));
	}

	void Material::SetTexture(const std::string& name, Texture2D* texture)
	{
		if (!texture)
		{
			LOG_WARNING("Material::SetTexture - Texture is null!");
			texture = Texture2D::GetDefault();
		}

		const auto materialBindings = GetMaterialBindings(m_Shader->GetSpecification());

		auto binding = Engine::GetBinding(materialBindings, name);

		if (binding.type != BindingType::Texture2D)
		{
			LOG_WARNING("Material::SetTexture - Parameter '{}' is not a texture!", name);
			return;
		}

		m_Textures[name] = texture;
		CreateBindGroup(); // recreate bind group to update texture
	}

	void Material::CreateUniformBuffer()
	{
		wgpu::BufferDescriptor bufferDesc{};
		bufferDesc.label = { "MaterialUniformBuffer", WGPU_STRLEN }; //TODO: add material name
		bufferDesc.size = m_Shader->GetMaterialUniformBufferSize();
		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
		m_UniformBuffer = GraphicsContext::GetDevice().createBuffer(bufferDesc);
	}

	void Material::CreateBindGroup()
	{
		ASSERT(m_Shader, "Material::CreateMaterialBindGroup - No shader set for material!");

		const auto materialBindings = GetMaterialBindings(m_Shader->GetSpecification());

		std::vector<wgpu::BindGroupEntry> entries;
		entries.reserve(materialBindings.size());

		for (const auto& binding : materialBindings)
		{
			wgpu::BindGroupEntry entry{};
			entry.binding = binding.binding;

			if (binding.type == BindingType::UniformBuffer)
			{
				entry.buffer = m_UniformBuffer;
				entry.offset = 0;
				entry.size = m_UniformData.size();
			}
			else if (binding.type == BindingType::Texture2D)
			{
				if (m_Textures.find(binding.name) == m_Textures.end())
				{
					m_Textures[binding.name] = Texture2D::GetDefault();
				}

				auto tex = m_Textures[binding.name];
				entry.textureView = tex->GetTextureView();
			}
			else if (binding.type == BindingType::Sampler)
			{
				entry.sampler = SamplerPool::GetSampler(m_TextureFilter, m_TextureWrap);
			}

			entries.push_back(entry);
		}

		wgpu::BindGroupDescriptor bindGroupDesc{};
		bindGroupDesc.label = { "MaterialBindGroup", WGPU_STRLEN }; //TODO: add material name
		bindGroupDesc.layout = m_Shader->GetMaterialBindGroupLayout();
		bindGroupDesc.entryCount = static_cast<uint32_t>(entries.size());
		bindGroupDesc.entries = entries.data();

		m_BindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
	}

	void Material::Bind(wgpu::RenderPassEncoder pass)
	{
		GraphicsContext::GetQueue().writeBuffer(m_UniformBuffer, 0, m_UniformData.data(), m_UniformData.size());
		pass.setBindGroup(GroupID::Material, m_BindGroup, 0, nullptr);
	}


	Material* Material::GetDefault()
	{
		if (!s_DefaultMaterial)
		{
			MaterialSpecification spec;
			spec.shader = ShaderImporter::LoadShader("Resources/Engine/Shaders/unlit.wgsl");
			s_DefaultMaterial = new Material(spec);
			s_DefaultMaterial->SetVec4("color", glm::vec4{ 1.0f, 0.0f, 1.0f, 1.0f });
		}

		return s_DefaultMaterial;
	}

}