#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace Engine
{
	class SceneAsset;

	class ENGINE_API Renderer
	{
	public:
		void Initialize();

		void SetFlags(RenderPassType flags) { m_EnabledPasses = flags; }
		void EnableFlag(RenderPassType flag) { m_EnabledPasses |= flag; }
		void DisableFlag(RenderPassType flag) { m_EnabledPasses &= ~flag; }
		RenderPassType GetEnabledFlags() const { return m_EnabledPasses; }

		void SetSize(float width, float height) { m_RenderContext.width = width; m_RenderContext.height = height; }
		void SetColorTarget(wgpu::TextureView colorView);
		void SetDepthTarget(wgpu::TextureView depthView);
		void SetEnvironmentTexture(Texture2D texture);
		const RenderContext& GetRenderContext() const { return m_RenderContext; }

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
		RenderContext m_RenderContext;
		RenderPassType m_EnabledPasses = RenderPassType::None;
	};
}