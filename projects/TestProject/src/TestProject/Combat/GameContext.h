#pragma once

#include "Grid.h"
#include <glm/glm.hpp>

namespace GameContext
{
	inline Grid grid;
	inline Tile* hoveredTile = nullptr;
	inline glm::vec2 mousePosition = glm::vec2{0.0f, 0.0f};
};