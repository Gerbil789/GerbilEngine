#pragma once

#include "Engine/Graphics/Texture/Texture.h"
#include <glm/glm.hpp>

namespace Engine
{
	class ENGINE_API Texture2D : public Texture
	{
	public:
		Texture2D() = default;
		Texture2D(const TextureSpecification& specification, const void* data);
		static Texture2D* GetDefault();
		static Texture2D* GetDefaultNormal();
	};

	class ENGINE_API SubTexture2D
	{
	public:
		SubTexture2D(const Texture2D& texture, const glm::vec2& min, const glm::vec2& max);

		const Texture2D& GetTexture() const { return m_Texture; }
		const glm::vec2& GetUVMin() const { return m_UVMin; }
		const glm::vec2& GetUVMax() const { return m_UVMax; }

		static SubTexture2D CreateFromGrid(const Texture2D& texture, const glm::ivec2& cellCoords, const glm::ivec2& cellSize, const glm::ivec2& spriteSize = { 1,1 });

	private:
		Texture2D m_Texture;
		glm::vec2 m_UVMin;
		glm::vec2 m_UVMax;
	};
}