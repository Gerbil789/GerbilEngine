#include "enginepch.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Asset/Serializer/ShaderParser.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"
#include "Engine/Core/Log.h"
#include <ranges>
#include <stdexcept>

namespace engine
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
		shaderModule = GraphicsContext::GetDevice().CreateShaderModule(&shaderDesc);

		specification = ShaderParser::Parse(source);

		constexpr uint32_t materialGroupIndex = 2;

		auto materialBindings = specification.bindings | std::views::filter([](const Binding& binding)
			{ return binding.group == materialGroupIndex; });

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
				materialUniformBufferSize = buffer.size;

				// if (buffer.arraySize > 1)
				// {
				// 	// *buffer.arraySize or buffer.arraySize.value()
				// }
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

		materialBindGroupLayout = GraphicsContext::GetDevice().CreateBindGroupLayout(&desc);
	}

	const std::vector<Binding> Shader::GetMaterialBindings() const
	{
		constexpr uint32_t materialGroupIndex = 2;
		std::vector<Binding> materialBindings; //TODO: cache this?

		for (const auto& binding : specification.bindings)
		{
			if (binding.group == materialGroupIndex)
			{
				materialBindings.push_back(binding);
			}
		}

		return materialBindings;
	}

	const Binding& Shader::GetBinding(const std::string& name) const
	{
		for (const auto& binding : specification.bindings)
		{
			if (binding.name == name)
				return binding;
		}

		LOG_ERROR("Binding not found ({})", name);
		static const Binding invalidBinding{};
		return invalidBinding;
	}

	const Binding& Shader::GetBinding(uint32_t group, uint32_t binding) const
	{
		for (const auto& b : specification.bindings)
		{
			if (b.group == group && b.binding == binding)
				return b;
		}

		LOG_ERROR("Binding not found (@group({}) @binding({}))", group, binding);
		static const Binding invalidBinding{};
		return invalidBinding;
	}



	Shader& ShaderHandle::Get() const
	{
		return AssetManager::GetAsset(*this);
	}
}