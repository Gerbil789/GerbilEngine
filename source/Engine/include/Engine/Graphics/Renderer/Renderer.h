#pragma once

#include "Engine/Graphics/Renderer/RenderPass.h"
#include <vector>

namespace engine
{
	class Renderer
	{
	public:
		void Initialize(uint32_t width, uint32_t height, std::vector<RenderPass> passes);
		void SetSize(uint32_t width, uint32_t height);
		void SetEnvironment(Texture2D texture); //TODO: move away from renderer?

		void RenderScene(const SceneAsset& scene, const CameraComponent& camera, wgpu::TextureView targetView);

	private:
		void CreateEnvironmentUniformBuffer();
		void CreateEnvironmentBindGroup();
		void CreateShadowTexture(); //TODO: move to shadow pass?

	private:
	  wgpu::Texture m_DepthTexture;
		wgpu::Texture m_ColorTexture;

		wgpu::Buffer m_ViewUniformBuffer;
		wgpu::Buffer m_ModelStorageBuffer;
		wgpu::Buffer m_EnvironmentUniformBuffer;

		FrameContext m_FrameContext;
		std::vector<RenderPass> m_Passes;
	};
}