#pragma once

#include "Engine/Graphics/ShaderSpecification.h"
#include <webgpu/webgpu_cpp.h>
#include <filesystem>

namespace engine
{
	struct Shader
	{
		ShaderSpecification specification;
		wgpu::ShaderModule shaderModule;
		wgpu::BindGroupLayout materialBindGroupLayout;
		size_t materialUniformBufferSize = 0;

		[[nodiscard]] static Shader Create(const std::filesystem::path& path);

		Shader() = default;
    ~Shader() = default;

		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		Shader(Shader&&) noexcept = default;
		Shader& operator=(Shader&&) noexcept = default;

		const std::vector<Binding> GetMaterialBindings() const;
		const Binding& GetBinding(const std::string& name) const;
		const Binding& GetBinding(uint32_t group, uint32_t binding) const;
	};
}