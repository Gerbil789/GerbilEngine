#pragma once

#include "Engine/Asset/AssetHandle.h"
#include "Engine/Graphics/ShaderSpecification.h"
#include <webgpu/webgpu_cpp.h>

namespace engine
{
	struct Shader
	{
		Shader() = default;
		Shader(const std::string& source);

		ShaderSpecification specification;
		wgpu::ShaderModule shaderModule;
		wgpu::BindGroupLayout materialBindGroupLayout;
		size_t materialUniformBufferSize = 0;

		const std::vector<Binding> GetMaterialBindings() const;
		const Binding& GetBinding(const std::string& name) const;
		const Binding& GetBinding(uint32_t group, uint32_t binding) const;
	};

	struct ShaderHandle : public AssetHandle
	{
		using AssetHandle::AssetHandle;
		Shader& Get() const;

		static const ShaderHandle Pink;
		static const ShaderHandle Flat;
		static const ShaderHandle UI;
		static const ShaderHandle Skybox;
		static const ShaderHandle Normal;
	};

	inline constexpr ShaderHandle ShaderHandle::Pink{ 0 };
	inline constexpr ShaderHandle ShaderHandle::Flat{ 1 };
	inline constexpr ShaderHandle ShaderHandle::UI{ 2 };
	inline constexpr ShaderHandle ShaderHandle::Skybox{ 3 };
	inline constexpr ShaderHandle ShaderHandle::Normal{ 4 };
}