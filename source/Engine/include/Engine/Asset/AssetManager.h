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

	template <> struct AssetTraits<TextureHandle> { using DataType = Texture2D; };
	template <> struct AssetTraits<MeshHandle> { using DataType = Mesh; };
	template <> struct AssetTraits<ShaderHandle> { using DataType = Shader; };
	template <> struct AssetTraits<MaterialHandle> { using DataType = Material; };
	template <> struct AssetTraits<MaterialInstanceHandle> { using DataType = MaterialInstance; };
	template <> struct AssetTraits<AudioClipHandle> { using DataType = AudioClip; };
	template <> struct AssetTraits<SceneHandle> { using DataType = Scene; };

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
			AssetDataMap<TextureHandle>,
			AssetDataMap<MeshHandle>,
			AssetDataMap<ShaderHandle>,
			AssetDataMap<MaterialHandle>,
			AssetDataMap<MaterialInstanceHandle>,
			AssetDataMap<AudioClipHandle>,
			AssetDataMap<SceneHandle>
		> m_LoadedAssets;

		inline static std::tuple<
			AssetSet<TextureHandle>,
			AssetSet<MeshHandle>,
			AssetSet<ShaderHandle>,
			AssetSet<MaterialHandle>,
			AssetSet<MaterialInstanceHandle>,
			AssetSet<AudioClipHandle>,
			AssetSet<SceneHandle>
		> m_DirtyAssets;
	};
}