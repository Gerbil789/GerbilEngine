#include "enginepch.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Asset/Serializer/ShaderParser.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"

namespace Engine
{
	Shader::Shader(const std::string& source)
	{
		wgpu::ShaderSourceWGSL shaderCodeDesc;
		shaderCodeDesc.chain.next = nullptr;
		shaderCodeDesc.chain.sType = wgpu::SType::ShaderSourceWGSL;
		shaderCodeDesc.code = { source.c_str(), WGPU_STRLEN};

		wgpu::ShaderModuleDescriptor shaderDesc;
		shaderDesc.label = { "Shader Module", WGPU_STRLEN };
		shaderDesc.nextInChain = &shaderCodeDesc.chain;
		m_ShaderModule = GraphicsContext::GetDevice().createShaderModule(shaderDesc);

		m_Specification = ShaderParser::GetSpecification(source);

		auto materialBindings = GetMaterialBindings(m_Specification);

		std::vector<wgpu::BindGroupLayoutEntry> layoutEntries;
		layoutEntries.reserve(materialBindings.size());

		for (const auto& binding : materialBindings)
		{
			wgpu::BindGroupLayoutEntry entry = wgpu::Default;
			entry.binding = binding.binding;
			entry.visibility = binding.visibility;

			switch (binding.type)
			{
			case BindingType::UniformBuffer:
				entry.buffer.type = wgpu::BufferBindingType::Uniform;
				entry.buffer.hasDynamicOffset = false;
				entry.buffer.minBindingSize = 0;
				m_MaterialUniformBufferSize = binding.size;
				break;

			case BindingType::Texture2D:
				entry.texture.sampleType = binding.textureSample;
				entry.texture.viewDimension = wgpu::TextureViewDimension::_2D;
				entry.texture.multisampled = false;
				entry.visibility = wgpu::ShaderStage::Fragment;
				break;

			case BindingType::Sampler:
				entry.sampler.type = binding.samplerType;
				entry.visibility = wgpu::ShaderStage::Fragment;
				break;

			default:
				LOG_WARNING("Unsupported binding type in material bind group layout creation");
				continue;
			}

			layoutEntries.push_back(entry);
		}

		wgpu::BindGroupLayoutDescriptor desc;
		desc.label = { "MaterialBindGroupLayout", WGPU_STRLEN};
		desc.entryCount = layoutEntries.size();
		desc.entries = layoutEntries.data();

		m_MaterialBindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(desc);
	}
}