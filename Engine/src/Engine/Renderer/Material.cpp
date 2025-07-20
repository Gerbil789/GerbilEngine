#include "enginepch.h"
#include "Material.h"
#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/Serializer.h"
#include "Engine/Renderer/Shaders/FlatColorShader.h"
#include "Engine/Renderer/Shaders/PhongShader.h"
#include "Engine/Renderer/Renderer.h"

namespace Engine
{
	void Material::SetShader(const Ref<Shader>& shader)
	{
		m_Shader = shader;

		CreateMaterialUniformBuffer();
		CreateMaterialBindGroup();
	}

	void Material::Bind(wgpu::RenderPassEncoder pass)
	{
		if (!m_Shader)
		{
			LOG_WARNING("Material::Bind - No shader set for material {0}", this->m_Path.string());
			return;
		}

		const auto& parameters = m_Shader->GetParameters();

		for (const auto& param : parameters)
		{
			auto it = m_Values.find(param.name);
			if (it == m_Values.end())
				continue; 

			const MaterialValue& val = it->second;
			uint8_t* dst = m_UniformData.data() + param.offset;

			switch (param.type)
			{
			case ShaderParamType::Float:
			{
				if (auto f = std::get_if<float>(&val))
					memcpy(dst, f, sizeof(float));
				break;
			}
			case ShaderParamType::Vec3:
			{
				if (auto v3 = std::get_if<glm::vec3>(&val))
					memcpy(dst, v3, sizeof(glm::vec3));
				break;
			}
			case ShaderParamType::Vec4:
			{
				if (auto v4 = std::get_if<glm::vec4>(&val))
					memcpy(dst, v4, sizeof(glm::vec4));
				break;
			}
			default:
				break;
			}
		}

		GraphicsContext::GetQueue().writeBuffer(m_MaterialUniformBuffer, 0, m_UniformData.data(), m_UniformData.size());
		pass.setBindGroup(GroupID::Material, m_MaterialBindGroup, 0, nullptr);
	}

	
	void Material::CreateMaterialUniformBuffer()
	{
		const auto& params = m_Shader->GetParameters();

		//filter out parameters that are not uniform types
		std::vector<ShaderParameter> uniformParams;
		for (const auto& param : params)
		{
			if (param.type == ShaderParamType::Float || param.type == ShaderParamType::Vec3 || param.type == ShaderParamType::Vec4)
			{
				uniformParams.push_back(param);
			}
		}

		wgpu::BufferDescriptor bufferDesc{};
		bufferDesc.label = { "MaterialUniformBuffer", WGPU_STRLEN };
		bufferDesc.size = m_Shader->GetUniformBufferSize();
		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;

		m_MaterialUniformBuffer = GraphicsContext::GetDevice().createBuffer(bufferDesc);
		m_UniformData.resize(m_Shader->GetUniformBufferSize(), 0);
	}

	void Material::CreateMaterialBindGroup()
	{
		const auto& parameters = m_Shader->GetParameters();
		std::vector<wgpu::BindGroupEntry> entries;
		entries.reserve(parameters.size());

		for (const auto& param : parameters)
		{
			wgpu::BindGroupEntry entry{};
			entry.binding = param.binding;

			switch (param.type)
			{
			case ShaderParamType::Float:
			case ShaderParamType::Vec3:
			case ShaderParamType::Vec4:
				entry.buffer = m_MaterialUniformBuffer;
				entry.offset = param.offset; 
				entry.size = param.size;
				break;
			case ShaderParamType::Texture2D:
				entry.textureView = m_Textures[param.name]->GetTextureView(); 
				break;

			case ShaderParamType::Sampler:
				entry.sampler = Renderer::s_Sampler; // Use a shared sampler instance
				break;
			}

			entries.push_back(entry);
		}

		wgpu::BindGroupDescriptor bindGroupDesc{};
		bindGroupDesc.label = { "MaterialBindGroup", WGPU_STRLEN };
		bindGroupDesc.layout = m_Shader->GetMaterialBindGroupLayout();
		bindGroupDesc.entryCount = static_cast<uint32_t>(entries.size());
		bindGroupDesc.entries = entries.data();

		m_MaterialBindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
	}

}