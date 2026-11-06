#include "enginepch.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Asset/Handle.h"
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

		auto processMap = []<typename Handle>(const std::unordered_map<uint64_t, std::string>&source)
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
				s_pathToId[fullPath.string()] = rawId;
			}
		};

		processMap.template operator() < TextureHandle > (archive.textures);
		processMap.template operator() < MeshHandle > (archive.meshes);
		processMap.template operator() < ShaderHandle > (archive.shaders);
		processMap.template operator() < MaterialHandle > (archive.materials);
		processMap.template operator() < MaterialInstanceHandle > (archive.materialInstances);
		processMap.template operator() < AudioClipHandle > (archive.audioClips);
		processMap.template operator() < SceneHandle > (archive.scenes);

		// look for new files
		for (const auto& entry : std::filesystem::recursive_directory_iterator(Project::assetsDirectory))
		{
			const std::filesystem::path& path = entry.path();

			if (!entry.is_regular_file() || !path.has_extension()) continue;

			std::string pathString = path.string();
			if (s_pathToId.contains(pathString)) continue;

			Uuid id = Uuid::Generate();
			AssetType type = GetAssetType(path);

			switch (type)
			{
			case AssetType::Texture:
				AddRecord(TextureHandle{ id }, path);
				break;

			case AssetType::Mesh:
				AddRecord(MeshHandle{ id }, path);
				break;

			case AssetType::Material:
				AddRecord(MaterialHandle{ id }, path);
				break;

			case AssetType::MaterialInstance:
				AddRecord(MaterialInstanceHandle{ id }, path);
				break;

			case AssetType::AudioClip:
				AddRecord(AudioClipHandle{ id }, path);
				break;

			case AssetType::Scene:
				AddRecord(SceneHandle{ id }, path);
				break;

			case AssetType::Shader:
				AddRecord(ShaderHandle{ id }, path);
				break;

			default:
				continue;
			}

			LOG_INFO("Discovered new asset '{}'", pathString);
			s_pathToId[pathString] = id;
		}



		Save();


		// LOG_INFO("--- Asset Registry Records ---");
    
    // auto printMap = []<typename Handle>()
    // {
    //   const auto& engineMap = std::get<AssetMap<Handle>>(s_records);
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

		auto fillArchive = []<typename Handle>(std::unordered_map<uint64_t, std::string>&dest)
		{
			const auto& engineMap = std::get<AssetMap<Handle>>(s_records);
			for (const auto& [handle, fullPath] : engineMap)
			{
				dest[static_cast<uint64_t>(handle.id)] = std::filesystem::relative(fullPath, Project::assetsDirectory).generic_string();
			}
		};

		fillArchive.template operator() < TextureHandle > (archive.textures);
		fillArchive.template operator() < MeshHandle > (archive.meshes);
		fillArchive.template operator() < ShaderHandle > (archive.shaders);
		fillArchive.template operator() < MaterialHandle > (archive.materials);
		fillArchive.template operator() < MaterialInstanceHandle > (archive.materialInstances);
		fillArchive.template operator() < AudioClipHandle > (archive.audioClips);
		fillArchive.template operator() < SceneHandle > (archive.scenes);

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
	// 	if (auto it = m_records.find(id); it != m_records.end())
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
		if (std::get<AssetMap<TextureHandle>>(s_records).contains(TextureHandle{ id }))
			return AssetType::Texture;

		if (std::get<AssetMap<MeshHandle>>(s_records).contains(MeshHandle{ id }))
			return AssetType::Mesh;

		if (std::get<AssetMap<ShaderHandle>>(s_records).contains(ShaderHandle{ id }))
			return AssetType::Shader;

		if (std::get<AssetMap<MaterialHandle>>(s_records).contains(MaterialHandle{ id }))
			return AssetType::Material;

		if (std::get<AssetMap<MaterialInstanceHandle>>(s_records).contains(MaterialInstanceHandle{ id }))
			return AssetType::MaterialInstance;

		if (std::get<AssetMap<AudioClipHandle>>(s_records).contains(AudioClipHandle{ id }))
			return AssetType::AudioClip;

		if (std::get<AssetMap<SceneHandle>>(s_records).contains(SceneHandle{ id }))
			return AssetType::Scene;

		return AssetType::Unknown;
	}
}