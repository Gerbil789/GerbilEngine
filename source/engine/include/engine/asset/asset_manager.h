#pragma once

#include "engine/asset/handle.h"
#include "engine/graphics/mesh.h"
#include "engine/graphics/shader.h"
#include "engine/graphics/texture/texture_2d.h"
#include "engine/graphics/material.h"
#include "engine/audio/audio_clip.h"
#include "engine/core/scene.h"
#include "engine/asset/asset_registry.h"
#include <filesystem>
#include <unordered_set>

namespace engine
{
	template <typename T>
	using assetDataMap = std::unordered_map<Handle<T>, T, assetHasher>;

	template <typename T>
	using assetSet = std::unordered_set<Handle<T>, assetHasher>;

	struct assetManager
	{
		static void Initialize();

		template <typename T>
		static T& Getasset(Handle<T> handle)
		{
			auto& map = std::get<assetDataMap<T>>(m_loadedassets);

			if (auto it = map.find(handle); it != map.end())
				return it->second;

			return map.at(Handle<T>{ 0 });
		}

		template<typename T>
		static Handle<T> Createasset(const std::filesystem::path& path);

		template<typename T>
		static void MarkassetDirty(Handle<T> handle)
		{
			if (assetRegistry::Exists(handle))
			{
				std::get<assetSet<T>>(m_dirtyassets).insert(handle);
			}
		}

		static void SaveDirtyassets();

	private:
		inline static std::tuple<
			assetDataMap<Texture2D>,
			assetDataMap<Mesh>,
			assetDataMap<Shader>,
			assetDataMap<Material>,
			assetDataMap<MaterialInstance>,
			assetDataMap<AudioClip>,
			assetDataMap<Scene>
		> m_loadedassets;

		inline static std::tuple<
			assetSet<Texture2D>,
			assetSet<Mesh>,
			assetSet<Shader>,
			assetSet<Material>,
			assetSet<MaterialInstance>,
			assetSet<AudioClip>,
			assetSet<Scene>
		> m_dirtyassets;
	};
}