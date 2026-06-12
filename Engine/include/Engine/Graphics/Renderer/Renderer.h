#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"
#include "Engine/Graphics/Texture/Texture2D.h"

namespace Engine
{
	class Scene;
	class TextureCube;

	class ENGINE_API Renderer
	{
	public:
		void Initialize();

		void SetFlags(RenderPassType flags) { m_EnabledPasses = flags; }
		void EnableFlag(RenderPassType flag) { m_EnabledPasses |= flag; }
		void DisableFlag(RenderPassType flag) { m_EnabledPasses &= ~flag; }
		RenderPassType GetEnabledFlags() const { return m_EnabledPasses; }

		void SetColorTarget(wgpu::TextureView colorView);
		void SetDepthTarget(wgpu::TextureView depthView);
		void SetEnvironmentTexture(Uuid textureId);
		const RenderContext& GetRenderContext() const { return m_RenderContext; }

		void RenderScene(Scene& scene, Camera& camera);
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