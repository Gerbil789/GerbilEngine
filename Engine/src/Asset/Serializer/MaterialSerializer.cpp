#include "enginepch.h"
#include "Engine/Asset/Serializer/MaterialSerializer.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/Resources.h"
#include "Engine/Core/Project.h"
#include <glaze/glaze.hpp>
#include <fstream>

namespace Engine
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
struct glz::meta<Engine::MaterialJSON> {
	using T = Engine::MaterialJSON;
	static constexpr auto value = object(
		"Shader", &T::Shader,
		"Attributes", &T::Attributes,
		"Textures", &T::Textures,
		"Filter", &T::Filter,
		"Wrap", &T::Wrap
	);
};

namespace Engine
{
	void MaterialSerializer::Serialize(const Material& material, const std::filesystem::path& path)
	{
		MaterialJSON outData;
		outData.Shader = static_cast<uint64_t>(material.GetShader());
		outData.Filter = static_cast<uint32_t>(material.GetTextureFilter());
		outData.Wrap = static_cast<uint32_t>(material.GetTextureWrap());

		// serialize uniform data
		for (const auto& [name, variantValue] : material.GetParameters())
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
		for (const auto& [name, texture] : material.GetTextures())
		{
			outData.Textures[name] = static_cast<uint64_t>(texture);
		}

		auto assetsDirectory = Project::GetActive().GetAssetsDirectory();
		auto writeError = glz::write_file_json < glz::opts{ .prettify = true } > (outData, (assetsDirectory / path).string(), std::string{});

		if (writeError)
		{
			LOG_ERROR("Failed to write material JSON: {}", glz::format_error(writeError));
		}
	}

	std::optional<Material> MaterialSerializer::Deserialize(const std::filesystem::path& path)
	{
		MaterialJSON data;
		std::string buffer;

		if (auto ec = glz::read_file_json(data, path.string(), buffer))
		{
			LOG_ERROR("JSON parse error in {}: {}", path, glz::format_error(ec, buffer));
			return std::nullopt;
		}

		MaterialSpecification spec;
		spec.shaderId = Uuid{ data.Shader };

		//TODO: wtf is this cast?
		spec.filter = static_cast<decltype(spec.filter)>(data.Filter);
		spec.wrap = static_cast<decltype(spec.wrap)>(data.Wrap);

		// Deserialize Attributes dynamically using glz::json_t
		for (auto& [name, node] : data.Attributes)
		{
			if (node.is_number())
			{
				// glz::json_t stores all numbers internally as doubles
				spec.parameters[name] = static_cast<float>(node.get<double>());
			}
			else if (node.is_array())
			{
				auto& arr = node.get_array();
				size_t size = arr.size();

				if (size == 2)
				{
					spec.parameters[name] = glm::vec2(arr[0].get<double>(), arr[1].get<double>());
				}
				else if (size == 3) // Instantly works now!
				{
					spec.parameters[name] = glm::vec3(arr[0].get<double>(), arr[1].get<double>(), arr[2].get<double>());
				}
				else if (size == 4)
				{
					spec.parameters[name] = glm::vec4(arr[0].get<double>(), arr[1].get<double>(), arr[2].get<double>(), arr[3].get<double>());
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
			spec.textures[name] = Uuid(id);
		}

		return Material(spec);
	}
}