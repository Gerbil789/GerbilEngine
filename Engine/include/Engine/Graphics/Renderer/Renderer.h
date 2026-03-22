#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace Engine
{
	class Camera;
	class Scene;

	class ENGINE_API Renderer
	{
	public:
		void AddPass(RenderPass* pass);
		void RemovePass(RenderPass* pass);

		void SetScene(Scene* scene);
		void SetCamera(Camera* camera);
		void Resize(uint32_t width, uint32_t height);
		void SetColorTarget(wgpu::TextureView color);

		void RenderScene();
		wgpu::TextureView GetTextureView() const;

	private:
		RenderContext m_RenderContext;
		std::vector<RenderPass*> m_Passes;
	};
}