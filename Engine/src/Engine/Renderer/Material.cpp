#include "enginepch.h"
#include "Material.h"
#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Renderer/WebGPUUtils.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Asset/AssetManager.h"

namespace Engine
{
	Material::Material(const Ref<Shader>& shader)
	{
		m_Shader = shader; 

		m_UniformData.resize(m_Shader->GetUniformBufferSize(), 0);
		CreateUniformBuffer(m_Shader->GetUniformBufferSize());
		CreateSampler();
		CreateBindGroup();
	}

	void Material::SetTexture(const std::string& name, Ref<Texture2D> texture)
	{
		if (!texture)
		{
			LOG_WARNING("Material::SetTexture - Texture is null!");
			return;
		}

		auto materialGroup = m_Shader->GetSpecification().GetBindGroup("Material");
		if (!materialGroup)
		{
			LOG_ERROR("Material::SetTexture - Shader has no 'Material' bind group!");
			return;
		}

		auto binding = materialGroup->GetBinding(name);

		if(!binding)
		{
			LOG_ERROR("Material::SetTexture - No binding for texture named '{}' in shader!", name);
			return;
		}
		
		if (binding->type != ShaderSpecification::Binding::Type::Texture)
		{
			LOG_WARNING("Material::SetTexture - Parameter '{}' is not a texture!", name);
			return;
		}

		m_Textures[name] = texture;
	}

	void Material::CreateUniformBuffer(size_t size)
	{
		wgpu::BufferDescriptor bufferDesc{};
		bufferDesc.label = { "MaterialUniformBuffer", WGPU_STRLEN }; //TODO: add material name
		bufferDesc.size = size;
		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
		m_UniformBuffer = GraphicsContext::GetDevice().createBuffer(bufferDesc);
	}

	void Material::CreateSampler()
	{
		wgpu::SamplerDescriptor samplerDesc = {};
		samplerDesc.label = { "MaterialSampler", WGPU_STRLEN }; //TODO: add material name
		samplerDesc.addressModeU = wgpu::AddressMode::Repeat;
		samplerDesc.addressModeV = wgpu::AddressMode::Repeat;
		samplerDesc.addressModeW = wgpu::AddressMode::ClampToEdge;
		samplerDesc.magFilter = wgpu::FilterMode::Linear;
		samplerDesc.minFilter = wgpu::FilterMode::Linear;
		samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
		samplerDesc.lodMinClamp = 0.0f;
		samplerDesc.lodMaxClamp = 1.0f;
		samplerDesc.compare = wgpu::CompareFunction::Undefined;
		samplerDesc.maxAnisotropy = 1;
		m_Sampler = GraphicsContext::GetDevice().createSampler(samplerDesc);
	}

	void Material::CreateBindGroup()
	{
		ASSERT(m_Shader, "Material::CreateMaterialBindGroup - No shader set for material!");

		auto materialGroup = m_Shader->GetSpecification().GetBindGroup("Material");
		if (!materialGroup)
		{
			LOG_ERROR("Material::SetTexture - Shader has no 'Material' bind group!");
			return;
		}

		std::vector<wgpu::BindGroupEntry> entries;
		entries.reserve(materialGroup->bindings.size());

		for (const auto& binding : materialGroup->bindings)
		{
			wgpu::BindGroupEntry entry{};
			entry.binding = binding.binding;

			if (binding.type == ShaderSpecification::Binding::Type::UniformBuffer)
			{
				entry.buffer = m_UniformBuffer;
				entry.offset = 0;
				entry.size = m_UniformData.size();
			}
			else if (binding.type == ShaderSpecification::Binding::Type::Texture)
			{
				LOG_WARNING("Material::CreateMaterialBindGroup - Texture bindings not implemented yet!");
				m_Textures[binding.label] = Renderer::GetDefaultWhiteTexture();
				auto tex = m_Textures[binding.label];
				entry.textureView = tex->GetTextureView();
			}
			else if (binding.type == ShaderSpecification::Binding::Type::Sampler)
			{
				entry.sampler = m_Sampler;
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

}