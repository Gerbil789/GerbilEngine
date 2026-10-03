#pragma once

#include "Engine/Asset/AssetHandle.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Core/Scene.h"

namespace RESOURCES
{
	namespace MESH
	{
		static constexpr engine::Mesh CUBE{ 1 };
		static constexpr engine::Mesh SPHERE{ 2 };
	}

	namespace TEXTURE
	{
		static constexpr engine::Texture2D WHITE{ 11 };
		static constexpr engine::Texture2D BLACK{ 12 };
		static constexpr engine::Texture2D NORMAL{ 13 };
		static constexpr engine::Texture2D HDR{ 14 };
		static constexpr engine::Texture2D EDITOR_ICONS{ 15 };
		static constexpr engine::Texture2D DEFAULT_FONT_ATLAS{ 16 };
		static constexpr engine::Texture2D EDITOR_THUMBNAIL_ATLAS{ 17 };
	}

	namespace SHADER
	{
		static constexpr engine::Shader PINK{ 21 };
		static constexpr engine::Shader FLAT{ 22 };
		static constexpr engine::Shader UI{ 23 };
		static constexpr engine::Shader SKYBOX{ 24 };
		static constexpr engine::Shader NORMAL{ 25 };
	}

	namespace MATERIAL
	{
		static constexpr engine::Material INVALID{ 31 };
	}

	namespace MATERIAL_INSTANCE
	{
		static constexpr engine::MaterialInstance WHITE{ 41 };
		static constexpr engine::MaterialInstance INVALID{ 42 };
	}

	namespace SCENE
	{
		static constexpr engine::Scene DEFAULT{ 51 };
	}

	namespace FONT
	{
		static constexpr engine::Uuid DEFAULT = engine::Uuid{ 61 };
	}
}