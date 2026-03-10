#pragma once

#include "Engine/Graphics/Texture.h"
#include "Engine/Graphics/SkyboxShader.h"

namespace Engine
{
	class Skybox
	{
	public:
		Skybox();
		//const SkyboxShader& GetShader() const { return m_SkyboxShader; }
		const SkyboxShader& GetEnvironmentShader() const { return m_EnvironmentShader; }
		wgpu::BindGroup GetBindGroup() const { return m_BindGroup; }

	private:
		void CreateBindGroup();

	private:
		CubeMapTexture* m_CubemapTexture;
		Texture* m_HDRTexture;
		//SkyboxShader m_SkyboxShader = SkyboxShader("Resources/Engine/shaders/skybox.wgsl");
		SkyboxShader m_EnvironmentShader = SkyboxShader("Resources/Engine/shaders/environment.wgsl");
		wgpu::BindGroup m_BindGroup;
	};

}