#include "enginepch.h"
#include "MaterialSerializer.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Serializer/YamlUtils.h"

namespace Engine
{
	void SerializeUniformBuffer(YAML::Emitter& out, const Binding& binding, const std::vector<uint8_t>& data)
	{
		for (const auto& param : binding.parameters)
		{
			switch (param.type)
			{
			case ShaderValueType::Float:
			{
				float value;
				std::memcpy(&value, data.data() + param.offset, sizeof(float));
				Engine::Yaml::Write(out, param.name, value);
				break;
			}
			case ShaderValueType::Vec3:
			{
				glm::vec3 value;
				std::memcpy(&value, data.data() + param.offset, sizeof(glm::vec3));
				Engine::Yaml::Write(out, param.name, value);
				break;
			}
			case ShaderValueType::Vec4:
			{
				glm::vec4 value;
				std::memcpy(&value, data.data() + param.offset, sizeof(glm::vec4));
				Engine::Yaml::Write(out, param.name, value);
				break;
			}
			default:
				LOG_ERROR("MaterialSerializer::Serialize - Unsupported uniform type for parameter '{}'", param.name);
			}
		}
	}

	void MaterialSerializer::Serialize(Material* material, const std::filesystem::path& path)
	{
		if (!material)
		{
			LOG_ERROR("MaterialSerializer::Serialize - Material is null, cannot serialize.");
			return;
		}

		auto shader = material->GetShader();
		if (!shader)
		{
			LOG_ERROR("MaterialSerializer::Serialize - Material has no shader, cannot serialize.");
			return;
		}

		auto shaderSpec = shader->GetSpecification(); 
		auto materialBindings = GetMaterialBindings(shaderSpec);

		YAML::Emitter out;
		{
			Yaml::Map root(out);

			Yaml::Write(out, "Shader", shader->id);

			// Sampler
			out << YAML::Key << "Sampler" << YAML::Value;
			{
				Yaml::Map sampler(out);
				Yaml::Write(out, "Filter", (uint32_t)material->GetTextureFilter());
				Yaml::Write(out, "Wrap", (uint32_t)material->GetTextureWrap());
			}

			// Attributes
			out << YAML::Key << "Attributes" << YAML::Value;
			{
				Engine::Yaml::Map attrs(out);
				for (const Binding& binding : materialBindings)
				{
					if (binding.type == BindingType::UniformBuffer)
						SerializeUniformBuffer(out, binding, material->GetUniformData());
				}
			}

			// Textures
			out << YAML::Key << "Textures" << YAML::Value;
			{
				Engine::Yaml::Map textures(out);
				for (const Binding& binding : materialBindings)
				{
					if (binding.type != BindingType::Texture2D) continue;

					auto texture = material->GetTexture(binding.name);
					Engine::Yaml::Write(out, binding.name, texture ? texture->id : UUID{0});
				}
			}
		}

		// Write to file
		std::ofstream fout(path);
		if (!fout.is_open())
		{
			LOG_ERROR("Failed to open file for serialization: {}", path);
			return;
		}
		fout << out.c_str();
		fout.close();
	}

	Material* MaterialSerializer::Deserialize(const std::filesystem::path& path)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch (const YAML::Exception& e)
		{
			LOG_ERROR("MaterialSerializer::Deserialize - YAML parse error in {}: {}", path, e.what());
			return nullptr;
		}

		auto shader = AssetManager::GetAsset<Shader>(UUID(data["Shader"].as<uint64_t>()));

		if(!shader)
		{
			LOG_ERROR("MaterialSerializer::Deserialize - Failed to load shader with ID: {}", data["Shader"].as<uint64_t>());
			return nullptr;
		}
		MaterialSpecification spec;
		spec.shader = shader;

		if (auto sampler = data["Sampler"])
		{
			Yaml::Read(sampler, "Filter", (uint32_t&)spec.filter);
			Yaml::Read(sampler, "Wrap", (uint32_t&)spec.wrap);
		}

		if (auto attributes = data["Attributes"])
		{
			for (auto it : attributes)
			{
				std::string name = it.first.as<std::string>();
				const YAML::Node& node = it.second;

				float f;
				glm::vec4 v;

				if (Yaml::Read(node, name, f))
				{
					spec.floatDefaults[name] = f;
				}
				else if (Yaml::Read(node, v))
				{
					spec.vec4Defaults[name] = v;
				}
				else
				{
					LOG_WARNING("Unknown property format for '{}'", name);
				}
			}
		}

		if (auto textures = data["Textures"])
		{
			for (auto it : textures)
			{
				std::string name = it.first.as<std::string>();
				spec.textureDefaults[name] = UUID(it.second.as<uint64_t>());
			}
		}

		return new Material(spec);
	}
}