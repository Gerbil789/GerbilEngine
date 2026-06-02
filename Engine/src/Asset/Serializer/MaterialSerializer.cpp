#include "enginepch.h"
#include "Engine/Asset/Serializer/MaterialSerializer.h"
#include "Engine/Asset/AssetManager.h"
//#include "Engine/Asset/AssetRegistry.h"
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
		auto shader = material.GetShader();
		auto materialBindings = shader.GetBindings();
		const auto& uniformData = material.GetUniformData();

		MaterialJSON outData;
		outData.Shader = (uint64_t)shader.id;
		outData.Filter = (uint32_t)material.GetTextureFilter();
		outData.Wrap = (uint32_t)material.GetTextureWrap();

		for (const Binding& binding : materialBindings)
		{
			if (binding.type == BindingType::UniformBuffer)
			{
				for (const auto& param : binding.parameters)
				{
					if (!param.name.empty() && param.name[0] == '_') // skip padding parameters
						continue;

					switch (param.type)
					{
					case ShaderValueType::Float:
					{
						float value;
						std::memcpy(&value, uniformData.data() + param.offset, sizeof(float));
						outData.Attributes[param.name] = value;
						break;
					}
					case ShaderValueType::Vec2:
					{
						glm::vec2 value;
						std::memcpy(&value, uniformData.data() + param.offset, sizeof(glm::vec2));
						outData.Attributes[param.name] = std::vector<float>{ value.x, value.y };
						break;
					}
					case ShaderValueType::Vec3:
					{
						glm::vec3 value;
						std::memcpy(&value, uniformData.data() + param.offset, sizeof(glm::vec3));
						outData.Attributes[param.name] = std::vector<float>{ value.x, value.y, value.z };
						break;
					}
					case ShaderValueType::Vec4:
					{
						glm::vec4 value;
						std::memcpy(&value, uniformData.data() + param.offset, sizeof(glm::vec4));
						outData.Attributes[param.name] = std::vector<float>{ value.x, value.y, value.z, value.w };
						break;
					}
					default:
						LOG_ERROR("MaterialSerializer::Serialize - Unsupported uniform type for parameter '{}'", param.name);
					}
				}
			}
			else if (binding.type == BindingType::Texture2D)
			{
				auto texture = material.GetTexture(binding.name);
				if (texture && texture->id)
				{
					outData.Textures[binding.name] = (uint64_t)texture->id;
				}
			}
		}


		auto assetsDirectory = Project::GetActive().GetAssetsDirectory();

		auto writeError = glz::write_file_json<glz::opts{.prettify = true}>(outData, (assetsDirectory / path).string(), std::string{});

		if(writeError)
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

		//if (!AssetManager::GetAssetRegistry().GetRecord(Uuid(data.Shader)).IsValid())
		//{
		//	data.Shader = RESOURCES::SHADER::DEFAULT;
		//}

		Engine::Shader& shader = Engine::AssetManager::GetAsset<Shader>(Uuid(data.Shader));

		MaterialSpecification spec;
		spec.shader = &shader;

		// Note: ensure spec.filter and spec.wrap can be safely cast from uint32_t
		// depending on how TextureFilter/TextureWrap are defined in your engine
		spec.filter = static_cast<decltype(spec.filter)>(data.Filter);
		spec.wrap = static_cast<decltype(spec.wrap)>(data.Wrap);

		// Deserialize Attributes dynamically using glz::json_t
		for (auto& [name, node] : data.Attributes)
		{
			if (node.is_number())
			{
				// glz::json_t stores all numbers internally as doubles
				spec.floatDefaults[name] = static_cast<float>(node.get<double>());
			}
			else if (node.is_array())
			{
				auto& arr = node.get_array();
				size_t size = arr.size();

				if (size == 2)
				{
					spec.vec2Defaults[name] = glm::vec2(
						static_cast<float>(arr[0].get<double>()),
						static_cast<float>(arr[1].get<double>())
					);
				}
				//else if (size == 3)
				//{
				//	// NOTE: I added vec3Defaults here. In your YAML code, you serialized Vec3 
				//	// but forgot to deserialize it. If `vec3Defaults` doesn't exist in your 
				//	// MaterialSpecification, you can comment this block out.
				//	spec.vec3Defaults[name] = glm::vec3(
				//		static_cast<float>(arr[0].get<double>()),
				//		static_cast<float>(arr[1].get<double>()),
				//		static_cast<float>(arr[2].get<double>())
				//	);
				//}
				else if (size == 4)
				{
					spec.vec4Defaults[name] = glm::vec4(
						static_cast<float>(arr[0].get<double>()),
						static_cast<float>(arr[1].get<double>()),
						static_cast<float>(arr[2].get<double>()),
						static_cast<float>(arr[3].get<double>())
					);
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
			//if(AssetManager::GetAssetRegistry().GetRecord(Uuid(id)).IsValid())
			//{
			//	spec.textureDefaults[name] = Uuid(id);
			//}

			spec.textureDefaults[name] = Uuid(id);
		}

		return Material(spec);
	}
}