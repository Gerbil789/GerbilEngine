#pragma once

#include "Engine/Asset/AssetHandle.h"

namespace RESOURCES
{
	namespace MESH
	{
		static constexpr engine::Mesh EMPTY{ 1 };
		static constexpr engine::Mesh CUBE{ 2 };
		static constexpr engine::Mesh SPHERE{ 3 };
	}

	namespace TEXTURE
	{
		static constexpr engine::Texture2D WHITE{ 11 };
		static constexpr engine::Texture2D BLACK{ 12 };
		static constexpr engine::Texture2D NORMAL{ 13 };
		static constexpr engine::Texture2D HDR{ 14 };
		static constexpr engine::Texture2D EDITOR_ICONS{ 15 };
		static constexpr engine::Texture2D DEFAULT_FONT_ATLAS{ 16 };
	}

	namespace SHADER
	{
		static constexpr engine::Shader DEFAULT{ 21 };
		static constexpr engine::Shader FLAT{ 22 };
		static constexpr engine::Shader UI{ 23 };
	}

	namespace MATERIAL
	{
		static constexpr engine::Material WHITE{ 31 };
		static constexpr engine::Material PINK{ 32 };
	}

	namespace SCENE
	{
		static constexpr engine::Scene DEFAULT{ 41 };
	}

	namespace FONT
	{
		static constexpr engine::Uuid DEFAULT = engine::Uuid{ 51 };
	}
}