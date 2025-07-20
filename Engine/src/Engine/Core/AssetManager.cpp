#include "enginepch.h"
#include "AssetManager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Renderer/Mesh.h"

namespace Engine::AssetManager
{
	void Initialize()
	{
		RegisterAssetType<Texture2D>(
			[](const std::filesystem::path& path) { return std::make_shared<Texture2D>(path); },
			[](const std::filesystem::path& path) { return std::make_shared<Texture2D>(path); }
		);
		RegisterAssetType<Material>(
			[](const std::filesystem::path& path) { return std::make_shared<Material>(path); },
			[](const std::filesystem::path& path) { return std::make_shared<Material>(path); }
		);
		RegisterAssetType<Scene>(
			[](const std::filesystem::path& path) { return std::make_shared<Scene>(path); },
			[](const std::filesystem::path& path) { return std::make_shared<Scene>(path); }
		);
		RegisterAssetType<Mesh>(
			[](const std::filesystem::path& path) { return std::make_shared<Mesh>(path); },
			[](const std::filesystem::path& path) { return std::make_shared<Mesh>(path); }
		);
	}

}