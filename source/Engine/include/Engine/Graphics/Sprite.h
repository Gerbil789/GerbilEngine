#pragma once

#include "Engine/Asset/AssetHandle.h"
#include <glm/glm.hpp>

namespace engine
{
	class Sprite
	{
	public:
		Sprite(Texture2D texture, const glm::vec2& min, const glm::vec2& max);

		Texture2D GetTexture() const { return m_Texture; }
		glm::vec2 GetUVMin() const { return m_UVMin; }
		glm::vec2 GetUVMax() const { return m_UVMax; }

		static Sprite CreateFromGrid(Texture2D texture, glm::ivec2 textureSize, glm::ivec2 cellCoords, glm::ivec2 cellSize);

	private:
		Texture2D m_Texture;
		glm::vec2 m_UVMin;
		glm::vec2 m_UVMax;
	};
}