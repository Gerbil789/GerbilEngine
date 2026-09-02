#include "enginepch.h"
#include "Engine/Asset/Serializer/MaterialSerializer.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Resources.h"
#include "Engine/Core/Project.h"
#include "Engine/Core/Log.h"
#include <glaze/glaze.hpp>
#include <fstream>

namespace engine
{
	struct MaterialJSON
	{
		uint64_t Shader = 0;

		std::map<std::string, glz::generic> Attributes;
		std::map<std::string, uint64_t> Textures;

		uint32_t Filter = 0;
		uint32_t Wrap = 0;
	};
}

template <>
struct glz::meta<engine::MaterialJSON> {
	using T = engine::MaterialJSON;
	static constexpr auto value = object(
		"Shader", &T::Shader,
		"Attributes", &T::Attributes,
		"Textures", &T::Textures,
		"Filter", &T::Filter,
		"Wrap", &T::Wrap
	);
};

namespace engine
{
	void MaterialSerializer::Serialize(Material material, const std::filesystem::path& path)
	{
		if(!material) 
		{
			LOG_ERROR("Attempted to serialize null material with ID '{}'", material.id);
			return;
		}

		const MaterialAsset& materialAsset = AssetManager::GetAsset(material);

		MaterialJSON outData;
		outData.Shader = static_cast<uint64_t>(materialAsset.GetShader());
		outData.Filter = static_cast<uint32_t>(materialAsset.GetTextureFilter());
		outData.Wrap = static_cast<uint32_t>(materialAsset.GetTextureWrap());

		// serialize uniform data
		for (const auto& [name, variantValue] : materialAsset.GetParameters())
		{
			if (!name.empty() && name[0] == '_') continue;

			std::visit([&](auto&& arg)
				{
					using T = std::decay_t<decltype(arg)>;

					// Glaze automatically understands primitive types and std::vector
					if constexpr (std::is_same_v<T, float>)
					{
						outData.Attributes[name] = arg;
					}
					else if constexpr (std::is_same_v<T, glm::vec2>)
					{
						outData.Attributes[name] = std::vector<float>{ arg.x, arg.y };
					}
					else if constexpr (std::is_same_v<T, glm::vec3>)
					{
						outData.Attributes[name] = std::vector<float>{ arg.x, arg.y, arg.z };
					}
					else if constexpr (std::is_same_v<T, glm::vec4>)
					{
						outData.Attributes[name] = std::vector<float>{ arg.x, arg.y, arg.z, arg.w };
					}
				}, variantValue);
		}

		// serialize textures
		for (const auto& [name, texture] : materialAsset.GetTextures())
		{
			outData.Textures[name] = static_cast<uint64_t>(texture);
		}

		auto writeError = glz::write_file_json < glz::opts{ .prettify = true } > (outData, (Project::AssetsDirectory() / path).string(), std::string{});

		if (writeError)
		{
			LOG_ERROR("Failed to write material JSON: {}", glz::format_error(writeError));
		}
	}

	std::optional<MaterialAsset> MaterialSerializer::Deserialize(const std::filesystem::path& path)
	{
		MaterialJSON data;
		std::string buffer;

		if (auto ec = glz::read_file_json(data, path.string(), buffer))
		{
			LOG_ERROR("JSON parse error in {}: {}", path, glz::format_error(ec, buffer));
			return std::nullopt;
		}

		MaterialSpecification spec;
		spec.shader = Shader{ data.Shader };

		MaterialAsset material = MaterialAsset(spec);

		material.SetTextureFilter(static_cast<TextureFilter>(data.Filter));
		material.SetTextureWrap(static_cast<TextureWrap>(data.Wrap));

		// Deserialize Attributes dynamically using glz::json_t
		for (auto& [name, node] : data.Attributes)
		{
			if (node.is_number())
			{
				// glz::json_t stores all numbers internally as doubles
				material.SetParameter(name, static_cast<float>(node.get<double>()));
			}
			else if (node.is_array())
			{
				auto& arr = node.get_array();
				size_t size = arr.size();

				if (size == 2)
				{
					material.SetParameter(name, glm::vec2(arr[0].get<double>(), arr[1].get<double>()));
				}
				else if (size == 3)
				{
					material.SetParameter(name, glm::vec3(arr[0].get<double>(), arr[1].get<double>(), arr[2].get<double>()));
				}
				else if (size == 4)
				{
					material.SetParameter(name, glm::vec4(arr[0].get<double>(), arr[1].get<double>(), arr[2].get<double>(), arr[3].get<double>()));
				}
				else
				{
					LOG_WARNING("Unknown property array format for '{}' (size: {})", name, size);
				}
			}
			else
			{
				LOG_WARNING("Unknown property JSON type for '{}'", name);
			}
		}

		// Deserialize Textures
		for (const auto& [name, id] : data.Textures)
		{
			material.SetTexture(name, Texture2D{ id });
		}

		return material;
	}
}