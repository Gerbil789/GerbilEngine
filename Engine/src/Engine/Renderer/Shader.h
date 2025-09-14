#pragma once

#include "Engine/Asset/Asset.h"
#include <filesystem>
#include <webgpu/webgpu.hpp>

namespace GroupID //TODO: remove
{
	constexpr int Frame = 0;
	constexpr int Model = 1;
	constexpr int Material = 2;
}

namespace Engine 
{
	struct ShaderSpecification
	{
		struct VertexAttribute
		{
			uint32_t location;
			std::string label;
			wgpu::VertexFormat format;
		};

		struct Binding 
		{
			uint32_t binding;
			std::string label;
			wgpu::ShaderStage stages;

			enum class Type
			{
				UniformBuffer,
				StorageBuffer,
				Texture,
				Sampler
			} type;

			union
			{
				wgpu::BufferBindingType bufferType;		// for Uniform / Storage
				wgpu::TextureSampleType textureType;	// for Texture
				wgpu::SamplerBindingType samplerType;	// for Sampler
			};
		};

		struct BindGroup 
		{
			uint32_t group;
			std::string label;							
			std::vector<Binding> bindings;

			const Binding* GetBinding(const std::string& name) const
			{
				for (const auto& b : bindings)
					if (b.label == name)
						return &b;
				return nullptr;
			}
		};

		std::string name;
		std::vector<VertexAttribute> vertexAttributes;
		std::vector<BindGroup> bindGroups;

		std::string vsEntry = "vs_main";
		std::string fsEntry = "fs_main";

		// GetBinding(string)
		const BindGroup* GetBindGroup(const std::string& name) const
		{ 
			for (const auto& group : bindGroups)
				if (group.label == name)
					return &group;
			return nullptr;
		} 
	};


	class Shader : public Asset
	{
	public:
		Shader(const ShaderSpecification& specification, const std::string& source);
		const ShaderSpecification& GetSpecification() const { return m_Specification; }
		wgpu::RenderPipeline GetRenderPipeline() const { return m_RenderPipeline; }
		wgpu::BindGroupLayout GetMaterialBindGroupLayout() const { return m_MaterialBindGroupLayout; }
		size_t GetUniformBufferSize() const { return m_CurrentUniformBufferOffset; }

	private:
		wgpu::ShaderModule CreateShaderModule(const std::string& source);
		wgpu::BindGroupLayout CreateMaterialBindGroupLayout(const ShaderSpecification& specification);

		size_t m_CurrentUniformBufferOffset = 0;

	private:
		ShaderSpecification m_Specification;
		wgpu::RenderPipeline m_RenderPipeline;
		wgpu::BindGroupLayout m_MaterialBindGroupLayout;
	};
}