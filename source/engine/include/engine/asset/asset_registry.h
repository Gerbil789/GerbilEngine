#pragma once

#include "engine/asset/asset_type.h"
#include "engine/asset/handle.h"
#include "engine/graphics/texture/texture_2d.h"
#include "engine/graphics/mesh.h"
#include "engine/graphics/shader.h"
#include "engine/graphics/material.h"
#include "engine/core/scene.h"
#include "engine/audio/audio_clip.h"
#include <unordered_map>

namespace engine
{
	template <typename T>
	using assetMap = std::unordered_map<T, std::filesystem::path, assetHasher>;

	struct assetRegistry
	{
		static void Load();
		static void Save();

		static Uuid GetId(const std::filesystem::path& path)
		{
			auto it = s_pathToId.find(path.string());
			if (it != s_pathToId.end())
			{
				return it->second;
			}
			return Uuid{};
		}

		template<typename Handle>
		static Handle Getasset(const std::filesystem::path& path)
		{
			Uuid id = s_pathToId.at(path.string());
			return Handle{ id };
		}

		template<typename Handle>
		static const std::filesystem::path& GetassetPath(Handle handle)
		{
			return std::get<assetMap<Handle>>(s_records).at(handle);
		}

		template<typename Handle>
		static void AddRecord(Handle handle, const std::filesystem::path& path)
		{
			std::get<assetMap<Handle>>(s_records)[handle] = path;
		}

		template<typename Handle>
		static bool Exists(Handle handle)
		{
			const auto& map = std::get<assetMap<Handle>>(s_records);
			return map.find(handle) != map.end();
		}

		template<typename Handle>
		static void RemoveRecord(Handle handle)
		{
			std::get<assetMap<Handle>>(s_records).erase(handle);
			Save();
		}

		static AssetType GetassetType(const std::filesystem::path& path);
		static AssetType GetassetType(Uuid id);

		template<typename T>
		static const assetMap<Handle<T>>& GetRecords()
		{
			return std::get<assetMap<Handle<T>>>(s_records);
		}

	private:
		inline static std::tuple<
			assetMap<TextureHandle>,
			assetMap<MeshHandle>,
			assetMap<ShaderHandle>,
			assetMap<MaterialHandle>,
			assetMap<MaterialInstanceHandle>,
			assetMap<AudioClipHandle>,
			assetMap<SceneHandle>
		> s_records;

		inline static std::unordered_map<std::string, Uuid> s_pathToId;
	};
}