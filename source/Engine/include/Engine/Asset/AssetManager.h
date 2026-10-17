#pragma once

#include "Engine/Asset/AssetHandle.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Core/Scene.h"
#include "Engine/Asset/AssetRegistry.h"
#include <filesystem>
#include <unordered_set>

namespace engine
{
	template <typename Handle> struct AssetTraits;

	template <> struct AssetTraits<Texture2D> { using DataType = Texture2DData; };
	template <> struct AssetTraits<Mesh> { using DataType = MeshAsset; };
	template <> struct AssetTraits<Shader> { using DataType = ShaderAsset; };
	template <> struct AssetTraits<Material> { using DataType = MaterialAsset; };
	template <> struct AssetTraits<MaterialInstance> { using DataType = MaterialInstanceData; };
	template <> struct AssetTraits<AudioClip> { using DataType = AudioClipAsset; };
	template <> struct AssetTraits<Scene> { using DataType = SceneAsset; };

	template <typename Handle>
	using AssetDataMap = std::unordered_map<Handle, typename AssetTraits<Handle>::DataType, AssetHasher>;

	template <typename Handle>
	using AssetSet = std::unordered_set<Handle, AssetHasher>;

	class AssetManager
	{
	public:
		static void Initialize();

		template <typename Handle>
		static auto& GetAsset(Handle handle)
		{
			auto& map = std::get<AssetDataMap<Handle>>(m_LoadedAssets);

			if (auto it = map.find(handle); it != map.end())
				return it->second;

			return map.at(Handle{ 0 });
		}

		template<typename Handle>
		static Handle CreateAsset(const std::filesystem::path& path);

		template<typename Handle>
		static void MarkAssetDirty(Handle handle)
		{
			if (AssetRegistry::Exists(handle))
			{
				std::get<AssetSet<Handle>>(m_DirtyAssets).insert(handle);
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
		> m_LoadedAssets;

		inline static std::tuple<
			AssetSet<Texture2D>,
			AssetSet<Mesh>,
			AssetSet<Shader>,
			AssetSet<Material>,
			AssetSet<MaterialInstance>,
			AssetSet<AudioClip>,
			AssetSet<Scene>
		> m_DirtyAssets;
	};
}