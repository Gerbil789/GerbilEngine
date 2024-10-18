#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Core/Asset.h"
#include "Engine/Renderer/Material.h"

namespace YAML 
{
	class Emitter;
	class Node;
}

// material serialization keys
constexpr const char* SHADER_KEY = "Shader";
constexpr const char* PROPERTIES_KEY = "Properties";

// scene serialization keys
//TODO: add scene serialization keys & update scene serialization
//NOTE: constexpr -> evaluated at compile time.

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


		static glm::vec4& ReadVec4(const YAML::Node& node, const std::string& key);
		static glm::vec3& ReadVec3(const YAML::Node& node, const std::string& key);
		static glm::vec2& ReadVec2(const YAML::Node& node, const std::string& key);
		static float& ReadFloat(const YAML::Node& node, const std::string& key);
		static int& ReadInt(const YAML::Node& node, const std::string& key);
		static std::string& ReadString(const YAML::Node& node, const std::string& key);

	};
}