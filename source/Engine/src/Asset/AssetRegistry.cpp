#include "enginepch.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/Project.h"
#include "Engine/Core/Log.h"
#include <glaze/glaze.hpp>
#include <fstream>

namespace engine
{
	constexpr uint32_t HashExtension(std::string_view str)
	{
		uint32_t hash = 0x811c9dc5;
		for (char c : str)
		{
			char lowerC = (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
			hash ^= static_cast<uint32_t>(lowerC);
			hash *= 0x01000193;
		}
		return hash;
	}

	struct AssetRegistryArchive
	{
		std::unordered_map<uint64_t, std::string> textures;
		std::unordered_map<uint64_t, std::string> meshes;
		std::unordered_map<uint64_t, std::string> shaders;
		std::unordered_map<uint64_t, std::string> materials;
		std::unordered_map<uint64_t, std::string> materialInstances;
		std::unordered_map<uint64_t, std::string> audioClips;
		std::unordered_map<uint64_t, std::string> scenes;
	};

	void AssetRegistry::Load()
	{
		const std::filesystem::path path = Project::directory / "assetRegistry.json";

		if (!std::filesystem::exists(path))
		{
			LOG_WARNING("Asset registry file '{}' does not exist, creating new one...", path);
			Save();
		}

		AssetRegistryArchive archive;
		std::string buffer;

		if (auto ec = glz::read_file_json(archive, path.string(), buffer))
		{
			LOG_ERROR("Registry Load Error: {}", glz::format_error(ec, buffer));
			return;
		}

		auto processMap = [&]<typename Handle>(const std::unordered_map<uint64_t, std::string>&source)
		{
			for (const auto& [rawId, relativePath] : source)
			{
				std::filesystem::path fullPath = Project::assetsDirectory / relativePath;

				if (!std::filesystem::exists(fullPath))
				{
					LOG_WARNING("Asset file '{}' does not exist, skipping...", fullPath);
					continue;
				}

				Handle handle{ rawId };
				AddRecord(handle, fullPath);
				s_PathToId[fullPath.string()] = rawId;
			}
		};

		processMap.template operator() < Texture2D > (archive.textures);
		processMap.template operator() < Mesh > (archive.meshes);
		processMap.template operator() < Shader > (archive.shaders);
		processMap.template operator() < Material > (archive.materials);
		processMap.template operator() < MaterialInstance > (archive.materialInstances);
		processMap.template operator() < AudioClip > (archive.audioClips);
		processMap.template operator() < Scene > (archive.scenes);

		// look for new files
		for (const auto& entry : std::filesystem::recursive_directory_iterator(Project::assetsDirectory))
		{
			const std::filesystem::path& path = entry.path();

			if (!entry.is_regular_file() || !path.has_extension()) continue;

			std::string pathString = path.string();
			if (s_PathToId.contains(pathString)) continue;

			Uuid id = Uuid::Generate();
			AssetType type = GetAssetType(path);

			switch (type)
			{
			case AssetType::Texture:
				AddRecord(Texture2D{ id }, path);
				break;

			case AssetType::Mesh:
				AddRecord(Mesh{ id }, path);
				break;

			case AssetType::Material:
				AddRecord(Material{ id }, path);
				break;

			case AssetType::MaterialInstance:
				AddRecord(MaterialInstance{ id }, path);
				break;

			case AssetType::AudioClip:
				AddRecord(AudioClip{ id }, path);
				break;

			case AssetType::Scene:
				AddRecord(Scene{ id }, path);
				break;

			case AssetType::Shader:
				AddRecord(Shader{ id }, path);
				break;

			default:
				continue;
			}

			LOG_INFO("Discovered new asset '{}'", pathString);
			s_PathToId[pathString] = id;
		}



		Save();


		// LOG_INFO("--- Asset Registry Records ---");
    
    // auto printMap = [&]<typename Handle>()
    // {
    //   const auto& engineMap = std::get<AssetMap<Handle>>(s_Records);
    //   for (const auto& [handle, fullPath] : engineMap)
    //   {
    //     LOG_INFO("ID: {}, Path: {}", handle.id, fullPath.string());
    //   }
    // };

    // printMap.template operator()<Texture2D>();
    // printMap.template operator()<Mesh>();
    // printMap.template operator()<Shader>();
    // printMap.template operator()<Material>();
    // printMap.template operator()<MaterialInstance>();
    // printMap.template operator()<AudioClip>();
    // printMap.template operator()<Scene>();
	}

	void AssetRegistry::Save()
	{
		AssetRegistryArchive archive;

		auto fillArchive = [&]<typename Handle>(std::unordered_map<uint64_t, std::string>&dest)
		{
			const auto& engineMap = std::get<AssetMap<Handle>>(s_Records);
			for (const auto& [handle, fullPath] : engineMap)
			{
				dest[static_cast<uint64_t>(handle.id)] = std::filesystem::relative(fullPath, Project::assetsDirectory).generic_string();
			}
		};

		fillArchive.template operator() < Texture2D > (archive.textures);
		fillArchive.template operator() < Mesh > (archive.meshes);
		fillArchive.template operator() < Shader > (archive.shaders);
		fillArchive.template operator() < Material > (archive.materials);
		fillArchive.template operator() < MaterialInstance > (archive.materialInstances);
		fillArchive.template operator() < AudioClip > (archive.audioClips);
		fillArchive.template operator() < Scene > (archive.scenes);

		const std::filesystem::path path = Project::directory / "assetRegistry.json";
		std::string buffer;

		if (auto ec = glz::write_file_json < glz::opts{ .prettify = true } > (archive, path.string(), buffer))
		{
			LOG_ERROR("Registry Save Error: {}", glz::format_error(ec, buffer));
		}
	}

	// std::filesystem::path AssetRegistry::GetRelativePath(const Uuid& id) const
	// {
	// 	static std::filesystem::path emptyPath;
	// 	if (auto it = m_Records.find(id); it != m_Records.end())
	// 	{
	// 		return std::filesystem::relative(it->second.path, Project::AssetsDirectory());
	// 	}
	// 	return emptyPath; //TODO: return {}? and test it!!!

	// }


	AssetType AssetRegistry::GetAssetType(const std::filesystem::path& path)
	{
		switch (HashExtension(path.extension().string()))
		{
		case HashExtension(".png"):
		case HashExtension(".jpg"):
		case HashExtension(".jpeg"):
		case HashExtension(".hdr"):
			return AssetType::Texture;

		case HashExtension(".gltf"):
		case HashExtension(".glb"):
			return AssetType::Mesh;

		case HashExtension(".mat"):
			return AssetType::Material;

		case HashExtension(".mi"):
			return AssetType::MaterialInstance;

		case HashExtension(".wav"):
		case HashExtension(".mp3"):
			return AssetType::AudioClip;

		case HashExtension(".scene"):
			return AssetType::Scene;

		case HashExtension(".wgsl"):
			return AssetType::Shader;

		default:
			return AssetType::Unknown;
		}
	}

	AssetType AssetRegistry::GetAssetType(Uuid id)
	{
		if (std::get<AssetMap<Texture2D>>(s_Records).contains(Texture2D{ id }))
			return AssetType::Texture;

		if (std::get<AssetMap<Mesh>>(s_Records).contains(Mesh{ id }))
			return AssetType::Mesh;

		if (std::get<AssetMap<Shader>>(s_Records).contains(Shader{ id }))
			return AssetType::Shader;

		if (std::get<AssetMap<Material>>(s_Records).contains(Material{ id }))
			return AssetType::Material;

		if (std::get<AssetMap<MaterialInstance>>(s_Records).contains(MaterialInstance{ id }))
			return AssetType::MaterialInstance;

		if (std::get<AssetMap<AudioClip>>(s_Records).contains(AudioClip{ id }))
			return AssetType::AudioClip;

		if (std::get<AssetMap<Scene>>(s_Records).contains(Scene{ id }))
			return AssetType::Scene;

		return AssetType::Unknown;
	}
}