#pragma once

#include "Engine/Core/UUID.h"
#include <optional>

struct Tile 
{
  int x = 0;
  int y = 0;

  Engine::Uuid entityId;
  bool isWalkable = true;

  bool IsOccupied() const { return entityId; }
};

class Grid 
{
public:
	Grid() = default;
  Grid(int width, int height);

  int GetWidth() const { return m_width; }
  int GetHeight() const { return m_height; }

  std::optional<Tile> GetTile(int x, int y);

  bool IsValidPosition(int x, int y) const;

private:
  int m_width = 0;
  int m_height = 0;
  std::vector<Tile> m_tiles;

  int GetIndex(int x, int y) const;
};