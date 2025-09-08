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
			Ref<Asset> asset = Project::GetActive()->GetAssetManager()->GetAsset(id);
			return std::static_pointer_cast<T>(asset);
		}

		template<typename T>
		static Ref<T> CreateAsset(std::filesystem::path path)
		{
			Ref<T> asset = Project::GetActive()->GetAssetManager()->CreateAsset<T>(path);
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
	};


}