#include "enginepch.h"
#include "Engine/Graphics/Sprite.h"

namespace Engine
{
	Sprite::Sprite(Uuid texture, const glm::vec2& min, const glm::vec2& max) : m_Texture(texture), m_UVMin(min), m_UVMax(max) {}

	Sprite Sprite::CreateFromGrid(Uuid texture, glm::ivec2 textureSize, glm::ivec2 cellCoords, glm::ivec2 cellSize)
	{
		glm::vec2 size = textureSize;

		glm::vec2 min
		{
				(cellCoords.x * cellSize.x) / size.x,
				(cellCoords.y * cellSize.y) / size.y
		};

		glm::vec2 max
		{
				((cellCoords.x + 1) * cellSize.x) / size.x,
				((cellCoords.y + 1) * cellSize.y) / size.y
		};

		return Sprite(texture, min, max);
	}
}