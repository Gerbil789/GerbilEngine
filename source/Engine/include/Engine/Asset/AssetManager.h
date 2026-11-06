#pragma once

#include "Engine/Asset/Handle.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Core/Scene.h"
#include "Engine/Asset/AssetRegistry.h"
#include <filesystem>
#include <unordered_set>

namespace engine
{
	template <typename T>
	using AssetDataMap = std::unordered_map<Handle<T>, T, AssetHasher>;

	template <typename T>
	using AssetSet = std::unordered_set<Handle<T>, AssetHasher>;

	class AssetManager
	{
	public:
		static void Initialize();

		template <typename T>
		static T& GetAsset(Handle<T> handle)
		{
			auto& map = std::get<AssetDataMap<T>>(m_loadedAssets);

			if (auto it = map.find(handle); it != map.end())
				return it->second;

			return map.at(Handle<T>{ 0 });
		}

		template<typename T>
		static Handle<T> CreateAsset(const std::filesystem::path& path);

		template<typename T>
		static void MarkAssetDirty(Handle<T> handle)
		{
			if (AssetRegistry::Exists(handle))
			{
				std::get<AssetSet<T>>(m_dirtyAssets).insert(handle);
			}
		}

		static void SaveDirtyAssets();

	private:
		inline static std::tuple<
			AssetDataMap<Texture2D>,
			AssetDataMap<Mesh>,
			AssetDataMap<Shader>,
			AssetDataMap<Material>,
			AssetDataMap<MaterialInstance>,
			AssetDataMap<AudioClip>,
			AssetDataMap<Scene>
		> m_loadedAssets;

		inline static std::tuple<
			AssetSet<Texture2D>,
			AssetSet<Mesh>,
			AssetSet<Shader>,
			AssetSet<Material>,
			AssetSet<MaterialInstance>,
			AssetSet<AudioClip>,
			AssetSet<Scene>
		> m_dirtyAssets;
	};
}