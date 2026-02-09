#pragma once

#include "Engine/Core/API.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/RenderPass/RenderPass.h"
#include "Engine/Graphics/Renderer/RenderContext.h"
#include <webgpu/webgpu.hpp>

namespace Engine
{
	class ENGINE_API Renderer
	{
	public:
		Renderer();
		~Renderer();

		void AddPass(RenderPass* pass);

		void SetScene(Scene* scene);
		void SetCamera(Camera* camera);
		void Resize(uint32_t width, uint32_t height);

		void RenderScene();

		wgpu::TextureView GetTextureView() const;

		void SetColorTarget(wgpu::TextureView color);

	private:
		RenderContext m_RenderContext;
		std::vector<RenderPass*> m_Passes;

	};
}