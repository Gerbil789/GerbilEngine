#pragma once

#include "Engine/Asset/AssetHandle.h"
#include "Engine/Graphics/ShaderSpecification.h"
#include <webgpu/webgpu_cpp.h>

namespace engine
{
	struct ShaderAsset
	{
		ShaderAsset() = default;
		ShaderAsset(const std::string& source);

		ShaderSpecification specification;
		wgpu::ShaderModule shaderModule;
		wgpu::BindGroupLayout materialBindGroupLayout;
		size_t materialUniformBufferSize = 0;
	};

	struct Shader : public AssetHandle
	{
		using AssetHandle::AssetHandle;
		ShaderAsset& Get() const;

		size_t GetMaterialUniformBufferSize() const;
		wgpu::BindGroupLayout GetMaterialBindGroupLayout() const;

		wgpu::ShaderModule GetShaderModule() const;
		const ShaderSpecification& GetSpecification() const;

		const std::vector<Binding> GetMaterialBindings() const;
		const Binding& GetBinding(const std::string& name) const;
		const Binding& GetBinding(uint32_t group, uint32_t binding) const;
	};
}