#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Graphics/ShaderSpecification.h"
#include <webgpu/webgpu.hpp>

namespace Engine 
{
	class Shader : public Asset
	{
	public:
		Shader() = default;
		Shader(const std::string& source);

		size_t GetMaterialUniformBufferSize() const { return m_MaterialUniformBufferSize; }
		wgpu::BindGroupLayout GetMaterialBindGroupLayout() const { return m_MaterialBindGroupLayout; }

		wgpu::ShaderModule GetShaderModule() const { return m_ShaderModule; }

		inline std::vector<Binding> GetBindings() const { return GetMaterialBindings(m_Specification); }
		inline const ShaderSpecification& GetSpecification() const { return m_Specification; }

	private:
		ShaderSpecification m_Specification;
		wgpu::ShaderModule m_ShaderModule;
		wgpu::BindGroupLayout m_MaterialBindGroupLayout;
		size_t m_MaterialUniformBufferSize = 0;
	};
}