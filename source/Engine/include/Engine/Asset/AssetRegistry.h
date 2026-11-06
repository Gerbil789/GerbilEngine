#pragma once

#include "Engine/Asset/AssetType.h"
#include "Engine/Asset/Handle.h"
#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Core/Scene.h"
#include "Engine/Audio/AudioClip.h"
#include <unordered_map>

namespace engine
{
	template <typename T>
	using AssetMap = std::unordered_map<T, std::filesystem::path, AssetHasher>;

	class AssetRegistry
	{
	public:
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
		static Handle GetAsset(const std::filesystem::path& path)
		{
			Uuid id = s_pathToId.at(path.string());
			return Handle{ id };
		}

		template<typename Handle>
		static const std::filesystem::path& GetAssetPath(Handle handle)
		{
			return std::get<AssetMap<Handle>>(s_records).at(handle);
		}

		template<typename Handle>
		static void AddRecord(Handle handle, const std::filesystem::path& path)
		{
			std::get<AssetMap<Handle>>(s_records)[handle] = path;
		}

		template<typename Handle>
		static bool Exists(Handle handle)
		{
			const auto& map = std::get<AssetMap<Handle>>(s_records);
			return map.find(handle) != map.end();
		}

		template<typename Handle>
		static void RemoveRecord(Handle handle)
		{
			std::get<AssetMap<Handle>>(s_records).erase(handle);
			Save();
		}

		static AssetType GetAssetType(const std::filesystem::path& path);
		static AssetType GetAssetType(Uuid id);

		template<typename T>
		static const AssetMap<Handle<T>>& GetRecords()
		{
			return std::get<AssetMap<Handle<T>>>(s_records);
		}

	private:
		inline static std::tuple<
			AssetMap<TextureHandle>,
			AssetMap<MeshHandle>,
			AssetMap<ShaderHandle>,
			AssetMap<MaterialHandle>,
			AssetMap<MaterialInstanceHandle>,
			AssetMap<AudioClipHandle>,
			AssetMap<SceneHandle>
		> s_records;

		inline static std::unordered_map<std::string, Uuid> s_pathToId;
	};
}