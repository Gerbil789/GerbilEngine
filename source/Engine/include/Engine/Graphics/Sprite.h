#pragma once

#include "Engine/Graphics/Texture/Texture2D.h"
#include <glm/glm.hpp>

namespace engine
{
	struct Sprite
	{
		Sprite() = default;
		Sprite(Texture2D texture, const glm::vec2& min, const glm::vec2& max);

		static Sprite CreateFromGrid(Texture2D texture, glm::ivec2 textureSize, glm::ivec2 cellCoords, glm::ivec2 cellSize);

		Texture2D texture{};
		glm::vec2 uvMin = {0.0f, 0.0f};
		glm::vec2 uvMax = {1.0f, 1.0f};
	};
}