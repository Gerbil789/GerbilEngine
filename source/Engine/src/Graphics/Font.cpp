#include "enginepch.h"
#include "Engine/Graphics/Font.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/Resources.h"
#include <glaze/glaze.hpp>

namespace Engine
{
	void FontManager::Initialize()
	{
		const std::filesystem::path path = "resources/fonts/atlas.json";

		std::string buffer;

		if (auto ec = glz::read_file_json < glz::opts{ .error_on_unknown_keys = false } > (s_JsonData, path.string(), buffer))
		{
			LOG_ERROR("Failed to load font file '{}': {}", path, glz::format_error(ec, buffer));
			throw std::runtime_error("Failed to load font file.");
		}

		s_TextureAtlas = RESOURCES::TEXTURE::DEFAULT_FONT_ATLAS; // Texture atlas loading is handled in asset manager, so we just set the UUID here.

		for (Font& font : s_JsonData.variants)
		{
			s_FontVariantMap[font.name] = &font;

			for (Glyph& glyph : font.glyphs)
			{
				font.glyphMap[glyph.unicode] = &glyph;
			}
		}
	}

	void FontManager::Shutdown()
	{

	}

	const Font* FontManager::GetFont(const std::string& name)
	{
		return s_FontVariantMap[name];
	}

}