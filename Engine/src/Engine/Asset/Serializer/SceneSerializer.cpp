#include "enginepch.h"
#include "SceneSerializer.h"
#include "Engine/Scene/Entity.h"
#include <yaml-cpp/yaml.h>

//https://github.com/TheCherno/Hazel/blob/asset-manager/Hazel/src/Hazel/Scene/SceneSerializer.cpp

namespace Engine
{
	void SceneSerializer::Serialize(const Ref<Scene>& scene, const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		auto entities = scene->GetEntities();
		for(auto ent : entities)
		{
			//SerializeEntity(out, entity);
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
	}

	Ref<Scene> SceneSerializer::Deserialize(const std::filesystem::path& path)
	{
		auto scene = CreateRef<Scene>();
		return scene;
	}
}