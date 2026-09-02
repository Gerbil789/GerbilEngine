#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace engine
{
	class SceneAsset;

	class Renderer
	{
	public:
		void Initialize();

		void SetFlags(RenderPassType flags) { m_EnabledPasses = flags; }
		void EnableFlag(RenderPassType flag) { m_EnabledPasses |= flag; }
		void DisableFlag(RenderPassType flag) { m_EnabledPasses &= ~flag; }
		RenderPassType GetEnabledFlags() const { return m_EnabledPasses; }

		void SetSize(float width, float height);
		void SetColorTarget(wgpu::TextureView colorView);
		void SetEnvironmentTexture(Texture2D texture);
		const RenderContext& GetRenderContext() const { return m_RenderContext; }

		void SetCamera(CameraComponent& camera, TransformComponent& transform)
		{
			m_RenderContext.camera = &camera;
			m_RenderContext.cameraTransform = &transform;
		}

		void RenderScene(SceneAsset& scene);

		wgpu::TextureView GetTextureView() const;

	private:
		void CreateViewUniformBuffer();
		void CreateViewBindGroup();

		void CreateModelStorageBuffer();
		void CreateModelBindGroup();

		void CreateEnvironmentUniformBuffer();
		void CreateEnvironmentBindGroup();

		void CreateShadowTexture(); //TODO: move to shadow pass?

	private:
	  wgpu::Texture m_DepthTexture;
		RenderContext m_RenderContext;
		RenderPassType m_EnabledPasses = RenderPassType::None;
	};
}