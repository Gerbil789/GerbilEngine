#pragma once

#include "Engine/Core/UUID.h"
#include "Engine/Asset/AssetRecord.h"
#include <string>
#include <filesystem>

namespace Engine
{
	class ENGINE_API Asset
	{
	public:
		Uuid id = Uuid{ 0 };

#ifdef GERBIL_EDITOR
		struct
		{
			std::string name;
			std::filesystem::path path;
			AssetType type = AssetType::Unknown;
		} EditorOnly;
#endif
	};
}