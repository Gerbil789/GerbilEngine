#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Graphics/ShaderSpecification.h"
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
	class Shader : public Asset
	{
	public:
		Shader(const ShaderSpecification& specification, const std::string& source, const std::string& name = "untitled");

		const std::string& GetName() const { return m_Name; }
		const ShaderSpecification& GetSpecification() const { return m_Specification; }
		wgpu::RenderPipeline GetRenderPipeline() const { return m_RenderPipeline; }
		size_t GetMaterialUniformBufferSize() const { return m_MaterialUniformBufferSize; }
		wgpu::BindGroupLayout GetMaterialBindGroupLayout() const { return m_MaterialBindGroupLayout; }

	private:
		wgpu::ShaderModule CreateShaderModule(const std::string& source);
		wgpu::BindGroupLayout CreateMaterialBindGroupLayout(const ShaderSpecification& specification);

		size_t m_MaterialUniformBufferSize = 0;

	private:
    std::string m_Name;
		ShaderSpecification m_Specification;
		wgpu::RenderPipeline m_RenderPipeline;
		wgpu::BindGroupLayout m_MaterialBindGroupLayout;
	};
}