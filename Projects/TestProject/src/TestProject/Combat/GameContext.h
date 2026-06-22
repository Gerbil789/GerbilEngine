#pragma once

#include "Grid.h"
#include <optional>

namespace GameContext
{
	inline Grid grid;
	inline std::optional<Tile> hoveredTile = std::nullopt;
};