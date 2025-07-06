#pragma once

namespace Engine 
{
	class RenderCommand
	{
	public:
		static void SetClearColor(const glm::vec4& color) {}
		static void Clear() {}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		static void DrawIndexed(/*const Ref<VertexArray>& vertexArray,*/ uint32_t indexCount = 0) { /*s_RendererAPI->DrawIndexed(vertexArray, indexCount);*/ }
	};
}
