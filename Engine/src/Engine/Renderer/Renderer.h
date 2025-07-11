#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneManager.h"
#include <webgpu/webgpu.hpp>

namespace Engine
{
	class Renderer : public ISceneObserver
	{
	public:
		Renderer();
		~Renderer();
		void OnSceneChanged(Ref<Engine::Scene> newScene) override { m_Scene = newScene; }

		void SetClearColor(const glm::vec4& color) { m_ClearColor = { color.r, color.g, color.b, color.a }; }
		void Resize(uint32_t width, uint32_t height);

		void BeginScene(const Camera& camera);
		void RenderScene();
		void EndScene();

		wgpu::TextureView GetTextureView() const { return m_OutputView; }

	private:
		Ref<Scene> m_Scene;

		uint32_t m_Width = 1280;
		uint32_t m_Height = 720;

		wgpu::Device m_Device;
		wgpu::Queue m_Queue;
		wgpu::TextureView m_OutputView;
		wgpu::CommandEncoder m_CommandEncoder;
		wgpu::RenderPassEncoder m_RenderPass;
		wgpu::Color m_ClearColor = { 0.05f, 0.05f, 0.05f, 1.0f };


		//TODO: decouple statistic meassurement from the renderer
		//struct RendererStatistics
		//{
		//	uint32_t DrawCalls = 0;
		//	uint32_t VertexCount = 0;
		//	uint32_t IndicesCount = 0;
		//};

		//static RendererStatistics GetStats();
		//static void ResetStats();
	};
}