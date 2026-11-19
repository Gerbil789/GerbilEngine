#include "enginepch.h"
#include "engine/asset/asset_registry.h"
#include "engine/asset/handle.h"
#include "engine/core/context.h"
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

	struct assetRegistryArchive
	{
		std::unordered_map<uint64_t, std::string> textures;
		std::unordered_map<uint64_t, std::string> meshes;
		std::unordered_map<uint64_t, std::string> shaders;
		std::unordered_map<uint64_t, std::string> materials;
		std::unordered_map<uint64_t, std::string> materialInstances;
		std::unordered_map<uint64_t, std::string> audioClips;
		std::unordered_map<uint64_t, std::string> scenes;
	};

	void assetRegistry::Load()
	{
		const std::filesystem::path path = engine::context::projectDirectory / "assetRegistry.json";

		if (!std::filesystem::exists(path))
		{
			LOG_WARNING("asset registry file '{}' does not exist, creating new one...", path);
			Save();
		}

		assetRegistryArchive archive;
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
				std::filesystem::path fullPath = engine::context::assetsDirectory / relativePath;

				if (!std::filesystem::exists(fullPath))
				{
					LOG_WARNING("asset file '{}' does not exist, skipping...", fullPath);
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
		for (const auto& entry : std::filesystem::recursive_directory_iterator(engine::context::assetsDirectory))
		{
			const std::filesystem::path& path = entry.path();

			if (!entry.is_regular_file() || !path.has_extension()) continue;

			std::string pathString = path.string();
			if (s_pathToId.contains(pathString)) continue;

			Uuid id = Uuid::Generate();
			AssetType type = GetassetType(path);

			switch (type)
			{
			case AssetType::texture:
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
	}

	void assetRegistry::Save()
	{
		assetRegistryArchive archive;

		auto fillArchive = []<typename Handle>(std::unordered_map<uint64_t, std::string>&dest)
		{
			const auto& engineMap = std::get<assetMap<Handle>>(s_records);
			for (const auto& [handle, fullPath] : engineMap)
			{
				dest[static_cast<uint64_t>(handle.id)] = std::filesystem::relative(fullPath, engine::context::assetsDirectory).generic_string();
			}
		};

		fillArchive.template operator() < TextureHandle > (archive.textures);
		fillArchive.template operator() < MeshHandle > (archive.meshes);
		fillArchive.template operator() < ShaderHandle > (archive.shaders);
		fillArchive.template operator() < MaterialHandle > (archive.materials);
		fillArchive.template operator() < MaterialInstanceHandle > (archive.materialInstances);
		fillArchive.template operator() < AudioClipHandle > (archive.audioClips);
		fillArchive.template operator() < SceneHandle > (archive.scenes);

		const std::filesystem::path path = engine::context::projectDirectory  / "assetRegistry.json";
		std::string buffer;

		if (auto ec = glz::write_file_json < glz::opts{ .prettify = true } > (archive, path.string(), buffer))
		{
			LOG_ERROR("Registry Save Error: {}", glz::format_error(ec, buffer));
		}
	}

	AssetType assetRegistry::GetassetType(const std::filesystem::path& path)
	{
		switch (HashExtension(path.extension().string()))
		{
		case HashExtension(".png"):
		case HashExtension(".jpg"):
		case HashExtension(".jpeg"):
		case HashExtension(".hdr"):
			return AssetType::texture;

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

	AssetType assetRegistry::GetassetType(Uuid id)
	{
		if (std::get<assetMap<TextureHandle>>(s_records).contains(TextureHandle{ id }))
			return AssetType::texture;

		if (std::get<assetMap<MeshHandle>>(s_records).contains(MeshHandle{ id }))
			return AssetType::Mesh;

		if (std::get<assetMap<ShaderHandle>>(s_records).contains(ShaderHandle{ id }))
			return AssetType::Shader;

		if (std::get<assetMap<MaterialHandle>>(s_records).contains(MaterialHandle{ id }))
			return AssetType::Material;

		if (std::get<assetMap<MaterialInstanceHandle>>(s_records).contains(MaterialInstanceHandle{ id }))
			return AssetType::MaterialInstance;

		if (std::get<assetMap<AudioClipHandle>>(s_records).contains(AudioClipHandle{ id }))
			return AssetType::AudioClip;

		if (std::get<assetMap<SceneHandle>>(s_records).contains(SceneHandle{ id }))
			return AssetType::Scene;

		return AssetType::Unknown;
	}
}