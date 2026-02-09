#pragma once

#include "Engine/Graphics/Texture.h"
#include "Engine/Graphics/SkyboxShader.h"

namespace Engine
{
	class Skybox
	{
	public:
		Skybox();
		const SkyboxShader& GetShader() const { return m_SkyboxShader; }
		wgpu::BindGroup GetBindGroup() const { return m_BindGroup; }

	private:
		void CreateBindGroup();

	private:
		CubeMapTexture* m_CubemapTexture;
		SkyboxShader m_SkyboxShader = SkyboxShader("Resources/Engine/shaders/skybox.wgsl");
		wgpu::BindGroup m_BindGroup;
	};

}