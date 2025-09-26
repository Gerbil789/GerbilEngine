#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Asset/Asset.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/Project.h"

namespace Engine
{
	class AssetManager
	{
	public:
		template<typename T>
		static Ref<T> GetAsset(UUID id)
		{
			Ref<T> asset = Project::GetActive()->GetAssetManager()->GetAsset<T>(id);
			return asset;
		}

		template<typename T, typename... Args>
		static Ref<T> CreateAsset(const std::filesystem::path& path, Args&&... args)
		{
			Ref<T> asset = Project::GetActive()->GetAssetManager()->CreateAsset<T>(path, std::forward<Args>(args)...);
			return asset;
		}

		static bool IsAssetValid(UUID id)
		{
			return Project::GetActive()->GetAssetManager()->IsAssetIdValid(id);
		}

		static bool IsAssetLoaded(UUID id)
		{
			return Project::GetActive()->GetAssetManager()->IsAssetLoaded(id);
		}

		//get metadata
		static const AssetMetadata* GetAssetMetadata(UUID id)
		{
			return Project::GetActive()->GetAssetManager()->GetAssetMetadata(id);
		}

		static void OnEvent(Event& e)
		{
			Project::GetActive()->GetAssetManager()->OnEvent(e);
		}
	};
}