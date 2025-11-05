#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Renderer/Texture.h"
//#include <glm/glm.hpp>

namespace Engine 
{
	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

		const Ref<Texture2D>& GetTexture() const { return m_Texture; }
		const std::array<glm::vec2, 4>& GetTexCoords() const { return m_TexCoords; }

    static Ref<SubTexture2D> CreateFromGrid(const Ref<Texture2D>& texture, const glm::ivec2& cellCoords, const glm::ivec2& cellSize, const glm::ivec2& spriteSize = { 1,1 })
    {
      glm::vec2 texSize = { (float)texture->GetWidth(), (float)texture->GetHeight() };

      glm::vec2 min 
      {
          (cellCoords.x * cellSize.x) / texSize.x,
          (cellCoords.y * cellSize.y) / texSize.y
      };

      glm::vec2 max 
      {
          ((cellCoords.x + spriteSize.x) * cellSize.x) / texSize.x,
          ((cellCoords.y + spriteSize.y) * cellSize.y) / texSize.y
      };

      return CreateRef<SubTexture2D>(texture, min, max);
    }
	private:
		Ref<Texture2D> m_Texture;
		std::array<glm::vec2, 4> m_TexCoords;
	};

}