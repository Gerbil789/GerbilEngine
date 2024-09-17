#pragma once

#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/SubTexture2D.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Scene/Components.h"

namespace Engine
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void EndScene();
		static void Flush();


		static void DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID);


		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, glm::vec2 tilingFactor = {1.0f, 1.0f}, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, glm::vec2 tilingFactor = { 1.0f, 1.0f }, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, glm::vec2 tilingFactor = { 1.0f, 1.0f }, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, glm::vec2 tilingFactor = { 1.0f, 1.0f }, const glm::vec4& color = glm::vec4(1.0f));


		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f), int entityId = -1);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, glm::vec2 tilingFactor = { 1.0f, 1.0f }, const glm::vec4& color = glm::vec4(1.0f), int entityId = -1);


		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, glm::vec2 tilingFactor = { 1.0f, 1.0f }, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, glm::vec2 tilingFactor = { 1.0f, 1.0f }, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, glm::vec2 tilingFactor = { 1.0f, 1.0f }, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, glm::vec2 tilingFactor = { 1.0f, 1.0f }, const glm::vec4& color = glm::vec4(1.0f));


		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t TriangleCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		static Statistics GetStats();
		static void ResetStats();
	private:
		static void FlushAndReset();
	};
}