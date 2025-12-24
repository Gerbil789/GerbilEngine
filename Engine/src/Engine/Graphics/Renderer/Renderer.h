#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/Renderer/RenderPass.h"
#include "Engine/Graphics/Renderer/RenderContext.h"
#include <webgpu/webgpu.hpp>


namespace Engine
{
	class Renderer
	{
	public:
		Renderer(uint32_t width, uint32_t height);
		~Renderer();

		void AddPass(RenderPass* pass) { m_Passes.push_back(pass); }

		void SetScene(Scene* scene) { m_Scene = scene; }
		void SetCamera(Camera* camera) { m_Camera = camera; }
		void Resize(uint32_t width, uint32_t height);

		void RenderScene();

		inline wgpu::TextureView GetTextureView() const { return m_RenderContext.colorTarget; }

	private:
		Scene* m_Scene;
		Camera* m_Camera;
		std::vector<RenderPass*> m_Passes;

		wgpu::CommandEncoder m_CommandEncoder;
		RenderContext m_RenderContext;
	};
}