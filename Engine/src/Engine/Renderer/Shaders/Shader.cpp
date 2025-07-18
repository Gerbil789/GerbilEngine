#include "enginepch.h"
#include "Shader.h"
#include "Engine/Utils/File.h"
#include "Engine/Renderer/GraphicsContext.h"

namespace Engine
{
	wgpu::ShaderModule Shader::LoadShader(const std::filesystem::path& path)
	{
		auto device = GraphicsContext::GetDevice();

		std::string content;
		if (!Engine::ReadFile(path, content))
		{
			LOG_ERROR("Failed to load shader. %s", path.string());
			return nullptr;
		}

		const char* shaderSource = content.c_str();

		wgpu::ShaderModuleDescriptor shaderDesc;
		wgpu::ShaderModuleWGSLDescriptor shaderCodeDesc;
		shaderCodeDesc.chain.next = nullptr;
		shaderCodeDesc.chain.sType = wgpu::SType::ShaderSourceWGSL;
		shaderDesc.nextInChain = &shaderCodeDesc.chain;
		shaderCodeDesc.code = { shaderSource, strlen(shaderSource) };
		return device.createShaderModule(shaderDesc);
	}

	void Shader::AddParameter(const std::string& name, uint32_t group, uint32_t binding, ShaderParamType type, size_t size)
	{
		// Align offset to 16 bytes
		if(type != ShaderParamType::Texture2D && type != ShaderParamType::Sampler) 
		{
			constexpr size_t UniformAlignment = 16;
			m_CurrentUniformBufferOffset = AlignTo(m_CurrentUniformBufferOffset, UniformAlignment);
		}

		ShaderParameter param;
		param.name = name;
		param.group = group;
		param.binding = binding;
		param.type = type;
		param.size = size;
		param.offset = m_CurrentUniformBufferOffset;
		m_Parameters.push_back(param);

		m_CurrentUniformBufferOffset += size;
	}

	wgpu::BindGroupLayout Shader::CreateMaterialBindGroupLayout()
	{
		std::vector<wgpu::BindGroupLayoutEntry> entries;

		for (const auto& param : m_Parameters) {
			if (param.group != GroupID::Material) continue;

			wgpu::BindGroupLayoutEntry entry = wgpu::Default;
			entry.binding = param.binding;
			entry.visibility = wgpu::ShaderStage::Fragment;

			switch (param.type) {
			case ShaderParamType::Float:
			case ShaderParamType::Vec3:
			case ShaderParamType::Vec4:
				entry.buffer.type = wgpu::BufferBindingType::Uniform;
				entry.buffer.minBindingSize = param.size;
				break;

			case ShaderParamType::Texture2D:
				entry.texture.sampleType = wgpu::TextureSampleType::Float;
				entry.texture.viewDimension = wgpu::TextureViewDimension::_2D;
				entry.texture.multisampled = false;
				break;

			case ShaderParamType::Sampler:
				entry.sampler.type = wgpu::SamplerBindingType::Filtering;
				break;
			}

			entries.push_back(entry);
		}

		wgpu::BindGroupLayoutDescriptor desc{};
		desc.label = { "MaterialBindGroupLayout", WGPU_STRLEN };
		desc.entryCount = static_cast<uint32_t>(entries.size());
		desc.entries = entries.data();

		return GraphicsContext::GetDevice().createBindGroupLayout(desc);
	}
}