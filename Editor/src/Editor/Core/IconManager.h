#pragma once

#include "Engine/Graphics/Texture/Texture2D.h"
#include <filesystem>

namespace Engine { enum class AssetType; }

namespace Editor
{
	enum class Icon //TODO: add invalid icon for debugging
	{
		EmptyFolder,
		Folder,
		File,
		EmptyFile,
		Audio,
		Landscape,
		Image,
		Mesh,
		Count
	};

	class IconManager
	{
	public:
		static void Initialize();
		static Engine::SubTexture2D& GetIcon(Icon icon);
		static Engine::SubTexture2D& GetIcon(Engine::AssetType assetType);
	};
}
