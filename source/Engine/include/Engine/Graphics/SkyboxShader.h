#pragma once

#include <filesystem>
#include <webgpu/webgpu.hpp>

namespace Engine
{
	class SkyboxShader
	{
	public:
		SkyboxShader() = default;
		void Initialize(const std::filesystem::path& path);

		wgpu::RenderPipeline GetRenderPipeline() const { return m_RenderPipeline; }

	private:
		wgpu::RenderPipeline m_RenderPipeline;
	};
}