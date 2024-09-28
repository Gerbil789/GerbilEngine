#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Core/Asset.h"
#include "Engine/Renderer/Material.h"

namespace YAML {
	class Emitter;
}

constexpr const char* SHADER_KEY = "shader";
constexpr const char* COLOR_TEXTURE_KEY = "colorTexture";
constexpr const char* METALLIC_TEXTURE_KEY = "metallicTexture";
constexpr const char* ROUGHNESS_TEXTURE_KEY = "roughnessTexture";
constexpr const char* NORMAL_TEXTURE_KEY = "normalTexture";
constexpr const char* HEIGHT_TEXTURE_KEY = "heightTexture";
constexpr const char* OCCLUSION_TEXTURE_KEY = "occlusionTexture";
constexpr const char* EMISSION_TEXTURE_KEY = "emissionTexture";
constexpr const char* COLOR_KEY = "color";
constexpr const char* METALLIC_KEY = "metallic";
constexpr const char* ROUGHNESS_KEY = "roughness";
constexpr const char* NORMAL_STRENGTH_KEY = "normalStrength";
constexpr const char* EMISSION_COLOR_KEY = "emissionColor";
constexpr const char* EMISSION_STRENGTH_KEY = "emission";
constexpr const char* TILING_KEY = "tiling";
constexpr const char* OFFSET_KEY = "offset";

namespace Engine
{
	class Serializer
	{
	public:
		static void Serialize(const Ref<Material>& material);
		static bool Deserialize(Ref<Material>& material);

		static void Serialize(const Ref<Scene>& scene);
		static bool Deserialize(Ref<Scene>& scene);

	private:
		static void WriteVec4(YAML::Emitter& out, const std::string& key, const glm::vec4& vec);
		static void WriteVec3(YAML::Emitter& out, const std::string& key, const glm::vec3& vec);
		static void WriteVec2(YAML::Emitter& out, const std::string& key, const glm::vec2& vec);
		static void WriteFloat(YAML::Emitter& out, const std::string& key, float value);
		static void WriteInt(YAML::Emitter& out, const std::string& key, int value);
		static void WriteString(YAML::Emitter& out, const std::string& key, const std::string& value);

	};
}