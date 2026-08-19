#pragma once

#include "Engine/Asset/AssetHandle.h"

namespace Engine
{
  struct Bounds 
  {
    float left = 0.0f;
    float bottom = 0.0f;
    float right = 0.0f;
    float top = 0.0f;
  };

  struct Glyph 
  {
    float advance = 0.0f;
    uint32_t unicode = 0;
    Bounds planeBounds;
    Bounds atlasBounds;
  };

  struct Kerning
  {
    uint32_t unicode1 = 0;
    uint32_t unicode2 = 0;
		float advanceOffset = 0.0f;
  };

  struct Metrics 
  {
    float ascender = 0.0f;
    float descender = 0.0f;
    float emSize = 0.0f;
    float lineHeight = 0.0f;
    float underlineThickness = 0.0f;
    float underlineY = 0.0f;
  };

  struct Font
  {
    std::string name;
    std::vector<Glyph> glyphs;
		std::vector<Kerning> kerning;
    Metrics metrics;



		std::unordered_map<uint32_t, Glyph*> glyphMap;

    Glyph* GetGlyph(uint32_t unicode) const
    {
      auto it = glyphMap.find(unicode);
      if (it != glyphMap.end())
      {
        return it->second;
      }
      return nullptr;
		}
  };

  struct AtlasMetadata
  {
    float distanceRange = 0.0f;
    float distanceRangeMiddle = 0.0f;
    float size = 0.0f;
    int width = 0;
    int height = 0;
  };

  struct FontAtlasJSON 
  {
    AtlasMetadata atlas;
		std::vector<Font> variants;
  };

	class ENGINE_API FontManager
	{
	public:
    static void Initialize();
    static void Shutdown();

    static const Font* GetFont(const std::string& name);
		static Texture2D GetFontAtlas() { return s_TextureAtlas; }

	private:
    inline static Texture2D s_TextureAtlas;
    inline static FontAtlasJSON s_JsonData;
    inline static std::unordered_map<std::string, Font*> s_FontVariantMap;
	};
}