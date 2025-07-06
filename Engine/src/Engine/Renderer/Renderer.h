#pragma once

#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Scene/Scene.h"
#include <webgpu/webgpu.hpp>

namespace Engine
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		//void SetClearColor(const glm::vec4& color) {}
		void SetScene(Ref<Scene> scene) { m_Scene = scene; }
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

		wgpu::Texture m_OutputTexture;
		wgpu::TextureView m_OutputView;

		wgpu::CommandEncoder m_CommandEncoder;
		wgpu::RenderPassEncoder m_RenderPass;

		wgpu::RenderPipeline m_Pipeline;

		//static void DrawMesh(const glm::mat4& transform, Ref<Mesh> mesh, Ref<Material> material, int entityID);


		//moved from rendererAPI
		
		//void Clear() {}
		//void DrawIndexed(/*const Ref<VertexArray>& vertexArray,*/ uint32_t indexCount = 0) {}
		//static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {}

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