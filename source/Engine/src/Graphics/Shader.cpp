#include "enginepch.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Asset/Serializer/ShaderParser.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"

namespace Engine
{
	Shader::Shader(const std::string& source)
	{
		wgpu::ShaderSourceWGSL shaderCodeDesc;
		shaderCodeDesc.nextInChain = nullptr;
		shaderCodeDesc.sType = wgpu::SType::ShaderSourceWGSL;
		shaderCodeDesc.code = source.c_str();

		wgpu::ShaderModuleDescriptor shaderDesc;
		shaderDesc.label = "Shader Module";
		shaderDesc.nextInChain = &shaderCodeDesc;
		m_ShaderModule = GraphicsContext::GetDevice().CreateShaderModule(&shaderDesc);

		m_Specification = ShaderParser::Parse(source);

		auto materialBindings = GetMaterialBindings();

		std::vector<wgpu::BindGroupLayoutEntry> layoutEntries;
		layoutEntries.reserve(std::ranges::distance(materialBindings));

		for (const auto& binding : materialBindings)
		{
			wgpu::BindGroupLayoutEntry entry;
			entry.binding = binding.binding;
			entry.visibility = binding.visibility;

			if (std::holds_alternative<BufferBinding>(binding.data))
			{
				const BufferBinding& buffer = std::get<BufferBinding>(binding.data);

				entry.buffer.type = buffer.type;
				entry.buffer.hasDynamicOffset = false;
				entry.buffer.minBindingSize = 0;
				m_MaterialUniformBufferSize = buffer.size;

				if (buffer.arraySize)
				{
					// *buffer.arraySize or buffer.arraySize.value()
				}
			}
			else if (std::holds_alternative<TextureBinding>(binding.data))
			{
				const TextureBinding& texture = std::get<TextureBinding>(binding.data);


				entry.texture.sampleType = texture.sampleType;
				entry.texture.viewDimension = texture.viewDimension;
				entry.texture.multisampled = texture.isMultisampled;
				entry.visibility = wgpu::ShaderStage::Fragment;
			}
			else if (std::holds_alternative<SamplerBinding>(binding.data))
			{
				const SamplerBinding& sampler = std::get<SamplerBinding>(binding.data);

				entry.sampler.type = sampler.type;
				entry.visibility = wgpu::ShaderStage::Fragment;
			}
			else
			{
				LOG_WARNING("Unsupported binding type in material bind group layout creation");
				continue;
			}

			layoutEntries.push_back(entry);
		}

		wgpu::BindGroupLayoutDescriptor desc;
		desc.label = "MaterialBindGroupLayout";
		desc.entryCount = layoutEntries.size();
		desc.entries = layoutEntries.data();

		m_MaterialBindGroupLayout = GraphicsContext::GetDevice().CreateBindGroupLayout(&desc);
	}
}