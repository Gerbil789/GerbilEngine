#pragma once

#include "Engine/Graphics/Texture/Texture2D.h"

namespace Engine
{
	class ENGINE_API Sprite
	{
	public:
		Sprite(Uuid texture, const glm::vec2& min, const glm::vec2& max);

		Uuid GetTexture() const { return m_Texture; }
		glm::vec2 GetUVMin() const { return m_UVMin; }
		glm::vec2 GetUVMax() const { return m_UVMax; }

		static Sprite CreateFromGrid(Uuid texture, glm::ivec2 textureSize, glm::ivec2 cellCoords, glm::ivec2 cellSize);

	private:
		Uuid m_Texture;
		glm::vec2 m_UVMin;
		glm::vec2 m_UVMax;
	};
}