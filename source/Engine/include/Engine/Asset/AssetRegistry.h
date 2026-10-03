#pragma once

#include "Engine/Asset/AssetType.h"
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
			auto it = s_PathToId.find(path.string());
			if (it != s_PathToId.end())
			{
				return it->second;
			}
			return Uuid{};
		}

		template<typename Handle>
		static Handle GetAsset(const std::filesystem::path& path)
		{
			Uuid id = s_PathToId.at(path.string());
			return Handle{ id };
		}

		template<typename Handle>
		static const std::filesystem::path& GetPath(Handle handle)
		{
			return std::get<AssetMap<Handle>>(s_Records).at(handle);
		}

		template<typename Handle>
		static void AddRecord(Handle handle, const std::filesystem::path& path)
		{
			std::get<AssetMap<Handle>>(s_Records)[handle] = path;
		}

		template<typename Handle>
		static bool Exists(Handle handle)
		{
			const auto& map = std::get<AssetMap<Handle>>(s_Records);
			return map.find(handle) != map.end();
		}

		template<typename Handle>
		static void RemoveRecord(Handle handle)
		{
			std::get<AssetMap<Handle>>(s_Records).erase(handle);
			Save();
		}

		template <typename Callable>
		static void FindAndDispatch(Uuid id, Callable&& callback)
		{
			std::apply([&](const auto&... typedMaps)
				{
					bool found = false;

					((!found && [&]()
						{
							for (const auto& [handle, fullPath] : typedMaps)
							{
								if (handle.id == id)
								{
									callback(handle);
									found = true;
									return;
								}
							}
						}()), ...);

				}, s_Records);
		}


		static AssetType GetAssetType(const std::filesystem::path& path);
		static AssetType GetAssetType(Uuid id);


		template<typename Handle>
		static const AssetMap<Handle>& GetRecords()
		{
			return std::get<AssetMap<Handle>>(s_Records);
		}

	private:
		inline static std::tuple<
			AssetMap<Texture2D>,
			AssetMap<Mesh>,
			AssetMap<Shader>,
			AssetMap<Material>,
			AssetMap<MaterialInstance>,
			AssetMap<AudioClip>,
			AssetMap<Scene>
		> s_Records;

		inline static std::unordered_map<std::string, Uuid> s_PathToId;
	};
}