#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Graphics/ShaderSpecification.h"
#include <webgpu/webgpu.hpp>
#include <ranges>

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
		const ShaderSpecification& GetSpecification() const { return m_Specification; }

		inline auto GetMaterialBindings() const
		{ 
			constexpr uint32_t materialGroupIndex = 2;

			return m_Specification.bindings | std::views::filter([](const Binding& binding) 
				{
					return binding.group == materialGroupIndex;
				});
		}

		inline const Binding& GetBinding(const std::string& name)
		{
			for (const auto& binding : m_Specification.bindings)
			{
				if (binding.name == name) return binding;
			}

			throw std::runtime_error("Binding not found: " + name); //TODO: better error handling
		}

	private:
		ShaderSpecification m_Specification;
		wgpu::ShaderModule m_ShaderModule;
		wgpu::BindGroupLayout m_MaterialBindGroupLayout;
		size_t m_MaterialUniformBufferSize = 0;
	};
}