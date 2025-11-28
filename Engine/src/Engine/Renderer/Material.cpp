#include "enginepch.h"
#include "Material.h"
#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Renderer/WebGPUUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Importer/ShaderImporter.h"

namespace Engine
{
	Material::Material(const Ref<Shader>& shader)
	{
		m_Shader = shader; 

		m_UniformData.resize(m_Shader->GetMaterialUniformBufferSize(), 0);
		CreateUniformBuffer();
		CreateBindGroup();
	}

	void Material::SetShader(const Ref<Shader>& shader)
	{
		m_Shader = shader;

		m_UniformData.resize(m_Shader->GetMaterialUniformBufferSize(), 0);
		CreateUniformBuffer();
		CreateBindGroup();
	}

	void Material::SetFloat(const std::string& paramName, float value)
	{
		auto binding = GetBinding(GetMaterialBindings(m_Shader->GetSpecification()), "uMaterialUniforms");
		if (binding.type != BindingType::UniformBuffer)
		{
			LOG_WARNING("Material::SetFloat - Parameter '{}' is not a uniform buffer!", "uMaterialUniforms");
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

	void Material::SetTexture(const std::string& name, Ref<Texture2D> texture)
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
				// check if texture is set for this binding
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


	Ref<Material> Material::GetDefault()
	{
		if (!s_DefaultMaterial)
		{
			Ref<Shader> shader = ShaderImporter::LoadShader("Resources/Engine/Shaders/unlit.shader");
			s_DefaultMaterial = CreateRef<Material>(shader);
			s_DefaultMaterial->SetVec4("color", glm::vec4(1.0f));
		}

		return s_DefaultMaterial;
	}

}