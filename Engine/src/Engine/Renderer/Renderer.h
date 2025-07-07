#pragma once

#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Material.h"
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
		void SetScene(Ref<Scene> scene) { m_Scene = scene; }
		void Resize(uint32_t width, uint32_t height);

		void BeginScene(const Camera& camera);
		void RenderScene();
		void EndScene();

		wgpu::TextureView GetTextureView() const { return m_OutputView; }

	private:
		wgpu::ShaderModule LoadShader(const std::string& path);

	private:
		Ref<Scene> m_Scene;

		uint32_t m_Width = 1280;
		uint32_t m_Height = 720;

		wgpu::Device m_Device;
		wgpu::Queue m_Queue;
		wgpu::Texture m_OutputTexture;
		wgpu::TextureView m_OutputView;
		wgpu::CommandEncoder m_CommandEncoder;
		wgpu::RenderPassEncoder m_RenderPass;
		wgpu::RenderPipeline m_Pipeline;
		wgpu::Buffer m_PointBuffer;
		wgpu::Buffer m_IndexBuffer;

		wgpu::Color m_ClearColor = { 0.05f, 0.05f, 0.05f, 1.0f };

		//uint32_t m_VertexCount;
		uint32_t m_IndexCount;
		//static void DrawMesh(const glm::mat4& transform, Ref<Mesh> mesh, Ref<Material> material, int entityID);

		//struct GlobalUniform
		//{
		//	glm::mat4 ViewProjection = glm::mat4(1.0f);
		//};

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