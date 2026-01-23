#pragma once

#include "Editor/Core/IconManager.h"
#include "Engine/Asset/AssetManager.h"
#include "ThumbnailRenderer.h"

namespace Editor
{
	inline Icon GetIconForExtension(const std::string& ext)
	{
		static const std::unordered_map<std::string, Icon> map = {
				{".png", Icon::Image},
				{".scene", Icon::Landscape},
				{".material", Icon::File},
				{".glb", Icon::Mesh},
				{".wav", Icon::Audio},
				{".mp3", Icon::Audio},
		};

		if (auto it = map.find(ext); it != map.end())
		{
			return it->second;
		}

		return Icon::File;
	}

	class AssetItem
	{
	public:
		Engine::UUID UUID;
		std::filesystem::path Path;
		std::string Name;
		bool IsDirectory = false;

		wgpu::TextureView Thumbnail;
		Engine::SubTexture2D* Icon;

		AssetItem(const Engine::UUID uuid, const std::filesystem::path& path) : UUID(uuid), Path(path), Name(path.filename().stem().string()), IsDirectory(std::filesystem::is_directory(path))
		{
			if (IsDirectory)
			{
				Icon = IconManager::GetIcon(std::filesystem::is_empty(path) ? Icon::EmptyFolder : Icon::Folder);
				return;
			}

			std::string ext = path.extension().string();
			Icon = IconManager::GetIcon(GetIconForExtension(ext));

			if (ext == ".png" || ext == ".jpg")
			{
				Thumbnail = Engine::AssetManager::GetAsset<Engine::Texture2D>(uuid)->GetTextureView();
			}
			else if (ext == ".material")
			{
				auto thumbnailRenderer = ThumbnailRenderer();
				thumbnailRenderer.Initialize();
				Thumbnail = thumbnailRenderer.Render(Engine::AssetManager::GetAsset<Engine::Material>(uuid));
			}
		}
	};

}