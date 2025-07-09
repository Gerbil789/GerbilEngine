#pragma once

#include "Engine/Core/Asset.h"
#include <webgpu/webgpu.hpp>

namespace Engine 
{
	class Shader : public Asset
	{
	public:
		Shader(const std::filesystem::path& path);

		wgpu::RenderPipeline GetRenderPipeline() const { return m_RenderPipeline; }

	private:
		wgpu::ShaderModule LoadShader(const std::filesystem::path& path);

	private:
		wgpu::RenderPipeline m_RenderPipeline;
	};

	class ShaderFactory : public IAssetFactory
	{
	public:
		virtual Ref<Asset> Load(const std::filesystem::path& path, const std::any& data) override { return CreateRef<Shader>(path); }
		virtual Ref<Asset> Create(const std::filesystem::path& path, const std::any& data) { return nullptr; }
	};
}