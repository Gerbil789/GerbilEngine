#pragma once

#include "Engine/Graphics/ShaderSpecification.h"
#include <filesystem>
#include <webgpu/webgpu.hpp>

namespace Engine
{
	class SkyboxShader
	{
	public:
		SkyboxShader(const std::filesystem::path& path);
		wgpu::RenderPipeline GetRenderPipeline() const { return m_RenderPipeline; }
		wgpu::BindGroupLayout GetBindGroupLayout() const { return m_BindGroupLayout; }

	private:
		wgpu::RenderPipeline m_RenderPipeline;
		wgpu::BindGroupLayout m_BindGroupLayout;
	};
}