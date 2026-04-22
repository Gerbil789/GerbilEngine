#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"
#include <glm/glm.hpp>

namespace Engine
{
	class Camera;
	class Scene;
	class CubeMapTexture;

	class ENGINE_API Renderer
	{
	public:
		void Initialize();

		void SetFlags(RenderPassType flags) { m_EnabledPasses = flags; }
		void EnableFlag(RenderPassType flag) { m_EnabledPasses |= flag; }
		void DisableFlag(RenderPassType flag) { m_EnabledPasses &= ~flag; }
		RenderPassType GetEnabledFlags() const { return m_EnabledPasses; }

		void SetCamera(Camera* camera);
		void SetColorTarget(wgpu::TextureView colorView);
		void SetDepthTarget(wgpu::TextureView depthView);
		CubeMapTexture* GetSkyboxCubemap() const { return m_RenderContext.environmentCubemap; }
		void SetSkyboxCubemap(CubeMapTexture* cubemap) { m_RenderContext.environmentCubemap = cubemap; CreateEnvironmentBindGroup();}
		RenderContext GetRenderContext() const { return m_RenderContext; }

		void RenderScene(Scene* scene);
		wgpu::TextureView GetTextureView() const;

	private:
		void CreateViewUniformBuffer();
		void CreateViewBindGroup();

		void CreateModelUniformBuffer();
		void CreateModelBindGroup();

		void CreateEnvironmentUniformBuffer();
		void CreateEnvironmentBindGroup();

		void CreateShadowTexture(); //TODO: move to shadow pass?

	private:
		RenderContext m_RenderContext;
		RenderPassType m_EnabledPasses = RenderPassType::None;
	};

	extern ENGINE_API Renderer *g_Renderer;
}