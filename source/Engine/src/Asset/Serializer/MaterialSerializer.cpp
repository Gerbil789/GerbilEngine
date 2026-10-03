#include "enginepch.h"
#include "Engine/Asset/Serializer/Serializer.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Asset/Resources.h"
#include "Engine/Core/Project.h"
#include "Engine/Core/Log.h"
#include <glaze/glaze.hpp>
#include <fstream>
#include <variant>
#include <cstring>
#include <algorithm>

template <> struct glz::meta<glm::vec2> { static constexpr auto value = glz::array(&glm::vec2::x, &glm::vec2::y); };
template <> struct glz::meta<glm::vec3> { static constexpr auto value = glz::array(&glm::vec3::x, &glm::vec3::y, &glm::vec3::z); };
template <> struct glz::meta<glm::vec4> { static constexpr auto value = glz::array(&glm::vec4::x, &glm::vec4::y, &glm::vec4::z, &glm::vec4::w); };
template <> struct glz::meta<glm::ivec2> { static constexpr auto value = glz::array(&glm::ivec2::x, &glm::ivec2::y); };

template <> struct glz::meta<glm::mat3> {
	static constexpr auto value = [](auto& m) -> auto& { return reinterpret_cast<std::array<float, 9>&>(m); };
};
template <> struct glz::meta<glm::mat4> {
	static constexpr auto value = [](auto& m) -> auto& { return reinterpret_cast<std::array<float, 16>&>(m); };
};

template <>
struct glz::meta<engine::TextureFilter>
{
	static constexpr std::array keys = { "point", "bilinear", "trilinear" };
	static constexpr std::array value = { engine::TextureFilter::Point, engine::TextureFilter::Bilinear, engine::TextureFilter::Trilinear };
};

template <>
struct glz::meta<engine::TextureWrap>
{
	static constexpr std::array keys = { "repeat", "clamp", "mirror" };
	static constexpr std::array value = { engine::TextureWrap::Repeat, engine::TextureWrap::Clamp, engine::TextureWrap::Mirror };
};

template <>
struct glz::meta<engine::Uuid> {
	using T = engine::Uuid;
	static constexpr auto value = [](auto& u) -> uint64_t& { return reinterpret_cast<uint64_t&>(u); };
};

template <>
struct glz::meta<engine::AssetHandle>
{
	using T = engine::AssetHandle;
	static constexpr auto value = [](auto& h) -> uint64_t& { return h.id; };
};

// --- Added Glaze reflection for Engine asset types ---
template <>
struct glz::meta<engine::Shader>
{
	using T = engine::Shader;
	static constexpr auto value = [](auto& s) -> decltype(s.id)& { return s.id; };
};

template <>
struct glz::meta<engine::Material>
{
	using T = engine::Material;
	static constexpr auto value = [](auto& m) -> decltype(m.id)& { return m.id; };
};

template <>
struct glz::meta<engine::MaterialInstance> {
	using T = engine::MaterialInstance;
	static constexpr auto value = [](auto& m) -> decltype(m.id)& { return m.id; };
};

template <>
struct glz::meta<engine::Texture2D>
{
	using T = engine::Texture2D;
	static constexpr auto value = [](auto& t) -> decltype(t.id)& { return t.id; };
};
// -----------------------------------------------------

namespace engine
{
	struct MaterialJSON
	{
		Shader shader;
	};

	struct MaterialInstanceJSON
	{
		Material material;

		std::map<std::string, glz::generic> attributes;
		std::map<std::string, Texture2D> textures;

		TextureFilter filter = TextureFilter::Point;
		TextureWrap wrap = TextureWrap::Repeat;
	};
}

namespace engine
{
	void Serializer::SerializeMaterial(Material material, const std::filesystem::path& path)
	{
		if (!material)
		{
			LOG_ERROR("Attempted to serialize null material (ID: {})", material.id);
			return;
		}

		const MaterialAsset& materialData = material.Get();

		MaterialJSON json;
		json.shader = materialData.shader;

		auto writeError = glz::write_file_json < glz::opts{ .prettify = true } > (json, path.string(), std::string{});

		if (writeError)
		{
			LOG_ERROR("Failed to write material JSON: {}", glz::format_error(writeError));
		}
	}

	MaterialAsset Serializer::DeserializeMaterial(const std::filesystem::path& path)
	{
		MaterialJSON json;
		std::string buffer;

		if (auto ec = glz::read_file_json(json, path.string(), buffer))
		{
			LOG_ERROR("JSON parse error in {}: {}", path, glz::format_error(ec, buffer));
			return {};
		}

		Shader shader{ json.shader };

		return MaterialAsset(shader);
	}

	void Serializer::SerializeMaterialInstance(MaterialInstance materialInstance, const std::filesystem::path& path)
	{
		if (!materialInstance)
		{
			LOG_ERROR("Attempted to serialize null material instance (ID: {})", materialInstance.id);
			return;
		}

		const MaterialInstanceData& instanceData = materialInstance.Get();
		const std::vector<std::byte> uniformData = instanceData.uniformData;
		const std::unordered_map<uint32_t, Texture2D>& textures = instanceData.textures;

		MaterialInstanceJSON json;
		json.material = instanceData.material;
		json.filter = instanceData.textureFilter;
		json.wrap = instanceData.textureWrap;

		Shader shader = instanceData.material.GetShader();

		for (const Binding& binding : shader.GetMaterialBindings())
		{
			if (std::holds_alternative<BufferBinding>(binding.data))
			{
				const BufferBinding& buffer = std::get<BufferBinding>(binding.data);

				if (buffer.type != wgpu::BufferBindingType::Uniform) continue;

				for (const ShaderParameter& param : buffer.parameters)
				{
					// FIXED: Use trailing return type to force all lambda branches to return the variant type
					auto extractedValue = std::visit([&](const auto& defaultVal) -> decltype(param.defaultValue) {
						using T = std::decay_t<decltype(defaultVal)>;
						T value{};

						if (param.offset + sizeof(T) <= uniformData.size())
						{
							std::memcpy(&value, uniformData.data() + param.offset, sizeof(T));
						}
						return value; // Implicitly converts 'value' into the Variant wrapper
						}, param.defaultValue);

					json.attributes[param.name] = extractedValue;
				}
			}
			else if (std::holds_alternative<TextureBinding>(binding.data))
			{
				json.textures[binding.name] = textures.at(binding.binding);
			}
		}

		auto writeError = glz::write_file_json < glz::opts{ .prettify = true } > (json, path.string(), std::string{});

		if (writeError)
		{
			LOG_ERROR("Failed to write material JSON: {}", glz::format_error(writeError));
		}
	}

	MaterialInstanceData Serializer::DeserializeMaterialInstance(const std::filesystem::path& path)
	{
		MaterialInstanceJSON json;
		std::string stringBuffer;

		if (auto ec = glz::read_file_json(json, path.string(), stringBuffer))
		{
			LOG_ERROR("JSON parse error in {}: {}", path, glz::format_error(ec, stringBuffer));
			return {};
		}

		Material material = json.material;
		Shader shader = material.GetShader();

		MaterialInstanceData instanceData = MaterialInstanceData(material);

		instanceData.textureFilter = json.filter;
		instanceData.textureWrap = json.wrap;

		const std::vector<Binding>& bindings = shader.GetMaterialBindings();

		auto it = std::find_if(bindings.begin(), bindings.end(), [&](const Binding& b)
			{
				return std::holds_alternative<BufferBinding>(b.data);
			});

		// FIXED: Return 'instanceData' instead of void
		if (it == bindings.end()) return instanceData;

		const Binding& foundBinding = *it;
		const BufferBinding& buffer = std::get<BufferBinding>(foundBinding.data);

		for (auto& [name, node] : json.attributes)
		{
			auto paramIt = std::find_if(buffer.parameters.begin(), buffer.parameters.end(), [&](const ShaderParameter& p)
				{
					return p.name == name;
				});

			if (paramIt == buffer.parameters.end()) continue;

			const ShaderParameter& param = *paramIt;

			if (param.offset + param.size > instanceData.uniformData.size())
			{
				instanceData.uniformData.resize(param.offset + param.size);
			}

			if (node.is_number())
			{
				float value = static_cast<float>(node.get<double>());
				std::memcpy(instanceData.uniformData.data() + param.offset, &value, param.size);
			}
			else if (node.is_array())
			{
				auto& arr = node.get_array();
				size_t size = arr.size();

				if (size == 2)
				{
					glm::vec2 value = glm::vec2(arr[0].get<double>(), arr[1].get<double>());
					std::memcpy(instanceData.uniformData.data() + param.offset, &value, param.size);
				}
				else if (size == 3)
				{
					glm::vec3 value = glm::vec3(arr[0].get<double>(), arr[1].get<double>(), arr[2].get<double>());
					std::memcpy(instanceData.uniformData.data() + param.offset, &value, param.size);
				}
				else if (size == 4)
				{
					glm::vec4 value = glm::vec4(arr[0].get<double>(), arr[1].get<double>(), arr[2].get<double>(), arr[3].get<double>());
					std::memcpy(instanceData.uniformData.data() + param.offset, &value, param.size);
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
		for (const auto& [name, texture] : json.textures)
		{
			for (const auto& b : bindings)
			{
				if (b.name == name)
				{
					instanceData.textures[b.binding] = texture;
				}
			}
		}

		return instanceData;
	}
}