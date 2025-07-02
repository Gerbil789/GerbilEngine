#pragma once

#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Material.h"

namespace Engine
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void EndScene();
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void DrawMesh(const glm::mat4& transform, Ref<Mesh> mesh, Ref<Material> material, int entityID);


		//moved from rendererAPI
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		void DrawIndexed(/*const Ref<VertexArray>& vertexArray,*/ uint32_t indexCount = 0) {}
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {}

		struct GlobalUniform
		{
			glm::mat4 ViewProjection = glm::mat4(1.0f);
		};


		struct RendererStatistics
		{
			uint32_t DrawCalls = 0;
			uint32_t VertexCount = 0;
			uint32_t IndicesCount = 0;
		};

		static RendererStatistics GetStats();
		static void ResetStats();

	private:
		static void AlignOffset(size_t& currentOffset, size_t alignment);


	};
}