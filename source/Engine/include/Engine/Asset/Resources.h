#pragma once

#include "Engine/Asset/AssetHandle.h"

namespace RESOURCES
{
	namespace MESH
	{
		static constexpr Engine::Mesh EMPTY{ 1 };
		static constexpr Engine::Mesh CUBE{ 2 };
		static constexpr Engine::Mesh SPHERE{ 3 };
	}

	namespace TEXTURE
	{
		static constexpr Engine::Texture2D WHITE{ 11 };
		static constexpr Engine::Texture2D BLACK{ 12 };
		static constexpr Engine::Texture2D NORMAL{ 13 };
		static constexpr Engine::Texture2D HDR{ 14 };
		static constexpr Engine::Texture2D EDITOR_ICONS{ 15 };
		static constexpr Engine::Texture2D DEFAULT_FONT_ATLAS{ 16 };
	}

	namespace SHADER
	{
		static constexpr Engine::Shader DEFAULT{ 21 };
		static constexpr Engine::Shader FLAT{ 22 };
		static constexpr Engine::Shader UI{ 23 };
	}

	namespace MATERIAL
	{
		static constexpr Engine::Material WHITE{ 31 };
		static constexpr Engine::Material PINK{ 32 };
	}

	namespace SCENE
	{
		static constexpr Engine::Scene DEFAULT{ 41 };
	}

	namespace FONT
	{
		static constexpr Engine::Uuid DEFAULT = Engine::Uuid{ 51 };
	}
}