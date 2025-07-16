#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneManager.h"
#include <webgpu/webgpu.hpp>

namespace Engine
{
	class Renderer
	{
	public:
		Renderer();

		void SetClearColor(const glm::vec4& color) { m_ClearColor = { color.r, color.g, color.b, color.a }; }
		void Resize(uint32_t width, uint32_t height);

		void BeginScene(const Camera& camera);
		void RenderScene();
		void EndScene();

		wgpu::TextureView GetTextureView() const { return m_TextureView; }

	private:
		Scene* m_Scene;

		wgpu::Device m_Device;
		wgpu::Queue m_Queue;
		wgpu::TextureView m_TextureView;
		wgpu::TextureView m_DepthView;
		wgpu::CommandEncoder m_CommandEncoder;
		wgpu::RenderPassEncoder m_RenderPass;
		wgpu::Color m_ClearColor = { 0.05f, 0.05f, 0.05f, 1.0f };
	};
}