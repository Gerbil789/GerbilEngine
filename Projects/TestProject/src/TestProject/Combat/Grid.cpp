#include "Grid.h"

Grid::Grid(int width, int height) : m_width(width), m_height(height)
{
  m_tiles.resize(m_width * m_height);

  for (int y = 0; y < m_height; ++y) 
  {
    for (int x = 0; x < m_width; ++x) 
    {
      int index = GetIndex(x, y);
      m_tiles[index].x = x;
      m_tiles[index].y = y;
    }
  }
}

std::optional<Tile> Grid::GetTile(int x, int y)
{
  if (!IsValidPosition(x, y)) return std::nullopt;
  return m_tiles[GetIndex(x, y)];
}

bool Grid::IsValidPosition(int x, int y) const
{
  return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

int Grid::GetIndex(int x, int y) const
{
  return y * m_width + x;
}
