#include "enginepch.h"
#include "engine/graphics/material.h"
#include "engine/graphics/graphics_context.h"
#include "engine/graphics/renderer/renderer.h"
#include "engine/graphics/texture/texture_2d.h"

namespace engine
{
	static wgpu::Buffer CreateUniformBuffer(std::string_view name, size_t size)
	{
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.label = name;
		bufferDesc.size = size;
		bufferDesc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
		return graphicsContext::GetDevice().CreateBuffer(&bufferDesc);
	}

	Material::Material(ShaderHandle shader)
	{
		this->shaderHandle = shader;

		PipelineSpecification spec;
		spec.shaderHandle = shader;
		pipeline = PipelineCache::GetPipeline(spec);
	}

	MaterialInstance::MaterialInstance(MaterialHandle material)
	{
		this->materialHandle = material;

		size_t bufferSize = material.Get().shaderHandle.Get().materialUniformBufferSize;
		uniformBuffer = CreateUniformBuffer("MaterialUniformBuffer", bufferSize);
	}

	void MaterialInstance::CreateBindGroup()
	{
		const Material& material = materialHandle.Get();
		const Shader& shader = material.shaderHandle.Get();

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
				entry.buffer = uniformBuffer;
				entry.offset = 0;
				entry.size = uniformData.size();
			}
			else if (std::holds_alternative<textureBinding>(binding.data))
			{
				// if (m_textures.find(binding.binding) == m_textures.end()) // TODO: is this redundant check?
				// {
				//textures[binding.binding] = texture2D::White;
				// }

				const Texture2D& texture = textures.at(binding.binding).Get();
				entry.textureView = texture.textureView;
			}
			else if (std::holds_alternative<SamplerBinding>(binding.data))
			{
				entry.sampler = SamplerPool::GetSampler({ textureFilter, textureWrap });
			}
		}

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.label = "MaterialBindGroup";
		bindGroupDesc.layout = shader.materialBindGroupLayout;
		bindGroupDesc.entryCount = entries.size();
		bindGroupDesc.entries = entries.data();
		bindGroup = graphicsContext::GetDevice().CreateBindGroup(&bindGroupDesc);
	}

	void MaterialInstance::SettextureFilter(TextureFilter filter)
	{
		textureFilter = filter;
		CreateBindGroup();
	}

	void MaterialInstance::SettextureWrap(TextureWrap wrap)
	{
		textureWrap = wrap;
		CreateBindGroup();
	}

	void MaterialInstance::Settexture(uint32_t bindingIndex, TextureHandle texture)
	{
		const Binding& binding = materialHandle.Get().shaderHandle.Get().GetBinding(2, bindingIndex);

		if (!std::holds_alternative<textureBinding>(binding.data))
		{
			LOG_WARNING("Parameter '{}' is not a texture!", binding.name);
			return;
		}

		if (texture == texture::White && binding.name.find("Normal") != std::string::npos)
		{
			texture = texture::Normal;
		}

		textures[bindingIndex] = texture;
		CreateBindGroup();
	}

	TextureHandle MaterialInstance::Gettexture(uint32_t bindingIndex) const
	{
		auto it = textures.find(bindingIndex);
		if (it != textures.end())
		{
			return it->second;
		}
		return {};
	}
}