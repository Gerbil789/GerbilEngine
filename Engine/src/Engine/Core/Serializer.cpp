#include "enginepch.h"
#include "Engine/Core/Serializer.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

namespace Engine 
{
	void Serializer::Serialize(const Ref<Material>& material)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Shader" << YAML::Value << material->shaderName;
		out << YAML::Key << "SurfaceType" << YAML::Value << (int)material->surfaceType;
		out << YAML::Key << "ColorTexture" << YAML::Value << ((material->colorTexture) ? material->colorTexture->GetFilePath() : "null");
		out << YAML::Key << "Color" << YAML::Value << YAML::Flow << YAML::BeginSeq << material->color.r << material->color.g << material->color.b << material->color.a << YAML::EndSeq;
		out << YAML::Key << "MetallicTexture" << YAML::Value << ((material->metallicTexture) ? material->metallicTexture->GetFilePath() : "null");
		out << YAML::Key << "Metallic" << YAML::Value << material->metallic;
		out << YAML::Key << "RoughnessTexture" << YAML::Value << ((material->roughnessTexture) ? material->roughnessTexture->GetFilePath() : "null");
		out << YAML::Key << "Roughness" << YAML::Value << material->roughness;
		out << YAML::Key << "NormalTexture" << YAML::Value << ((material->normalTexture) ? material->normalTexture->GetFilePath() : "null");
		out << YAML::Key << "NormalStrength" << YAML::Value << material->normalStrength;
		out << YAML::Key << "Tiling" << YAML::Value << YAML::Flow << YAML::BeginSeq << material->tiling.x << material->tiling.y << YAML::EndSeq;
		out << YAML::Key << "Offset" << YAML::Value << YAML::Flow << YAML::BeginSeq << material->offset.x << material->offset.y << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(material->GetFilePath());
		fout << out.c_str();
		fout.close();
	}

	bool Serializer::Deserialize(Ref<Material>& material)
	{
		std::string filePath = material->GetFilePath();
		std::ifstream stream(filePath);
		if (!stream.is_open())
		{
			ENGINE_LOG_ERROR("Failed to open file '{0}'", filePath);
			return false;
		}

		YAML::Node data = YAML::LoadFile(filePath);
		if (!data["Shader"]) return false; //TODO: is this necessary?


		//TODO: make oneliners
		material->shaderName = data["Shader"].as<std::string>();
		material->surfaceType = (SurfaceType)data["SurfaceType"].as<int>();
		std::string colorTexturePath = data["ColorTexture"].as<std::string>();
		if(colorTexturePath != "null") material->colorTexture = AssetManager::GetAsset<Texture2D>(colorTexturePath);
		material->color = glm::vec4(data["Color"][0].as<float>(), data["Color"][1].as<float>(), data["Color"][2].as<float>(), data["Color"][3].as<float>());
		std::string metallicTexturePath = data["MetallicTexture"].as<std::string>();
		if (metallicTexturePath != "null") material->metallicTexture = AssetManager::GetAsset<Texture2D>(metallicTexturePath);
		material->metallic = data["Metallic"].as<float>();
		std::string roughnessTexturePath = data["RoughnessTexture"].as<std::string>();
		if (roughnessTexturePath != "null") material->roughnessTexture = AssetManager::GetAsset<Texture2D>(roughnessTexturePath);
		material->roughness = data["Roughness"].as<float>();
		std::string normalTexturePath = data["NormalTexture"].as<std::string>();
		if (normalTexturePath != "null") material->normalTexture = AssetManager::GetAsset<Texture2D>(normalTexturePath);
		material->normalStrength = data["NormalStrength"].as<float>();
		material->tiling = glm::vec2(data["Tiling"][0].as<float>(), data["Tiling"][1].as<float>());
		material->offset = glm::vec2(data["Offset"][0].as<float>(), data["Offset"][1].as<float>());

		return true;
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		ASSERT(entity.HasComponent<IDComponent>(), "Entity has no IdComponent!");

		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();
		out << YAML::Key << "Name" << YAML::Value << entity.GetComponent<NameComponent>().Name;
		out << YAML::Key << "Enabled" << YAML::Value << entity.GetComponent<EnablingComponent>().Enabled;
		out << YAML::Key << "Transform";

		out << YAML::BeginMap;
		auto& tc = entity.GetComponent<TransformComponent>();
		out << YAML::Key << "Position" << YAML::Flow << YAML::BeginSeq << tc.Position.x << tc.Position.y << tc.Position.z << YAML::EndSeq;
		out << YAML::Key << "Rotation" << YAML::Flow << YAML::BeginSeq << tc.Rotation.x << tc.Rotation.y << tc.Rotation.z << YAML::EndSeq;
		out << YAML::Key << "Scale" << YAML::Flow << YAML::BeginSeq << tc.Scale.x << tc.Scale.y << tc.Scale.z << YAML::EndSeq;
		out << YAML::EndMap;

		// Camera Component
		if (entity.HasComponent<CameraComponent>()) 
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;
			auto& cc = entity.GetComponent<CameraComponent>();
			auto& camera = cc.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap;

			out << YAML::Key << "Main" << YAML::Value << cc.Main;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.FixedAspectRatio;
			out << YAML::EndMap;
		}

		// Sprite Renderer Component
		if (entity.HasComponent<SpriteRendererComponent>()) 
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;
			auto& src = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Material" << YAML::Value << (src.Material ? src.Material->GetFilePath() : "null");
			out << YAML::Key << "Texture" << YAML::Value << (src.Texture ? src.Texture->GetFilePath() : "null");
			out << YAML::Key << "Color" << YAML::Flow << YAML::BeginSeq << src.Color.r << src.Color.g << src.Color.b << src.Color.a << YAML::EndSeq;
			out << YAML::Key << "TilingFactor" << YAML::Flow << YAML::BeginSeq << src.TilingFactor.x << src.TilingFactor.y << YAML::EndSeq;
			out << YAML::EndMap;
		}

		// Light Component
		if (entity.HasComponent<LightComponent>()) 
		{
			out << YAML::Key << "LightComponent";
			out << YAML::BeginMap;
			auto& lc = entity.GetComponent<LightComponent>();
			out << YAML::Key << "Type" << YAML::Value << (int)lc.Type;
			out << YAML::Key << "Color" << YAML::Flow << YAML::BeginSeq << lc.Color.r << lc.Color.g << lc.Color.b << YAML::EndSeq;
			out << YAML::Key << "Intensity" << YAML::Value << lc.Intensity;
			out << YAML::Key << "Range" << YAML::Value << lc.Range;
			out << YAML::Key << "Attenuation" << YAML::Flow << YAML::BeginSeq << lc.Attenuation.x << lc.Attenuation.y << lc.Attenuation.z << YAML::EndSeq;
			out << YAML::Key << "InnerAngle" << YAML::Value << lc.InnerAngle;
			out << YAML::Key << "OuterAngle" << YAML::Value << lc.OuterAngle;
			out << YAML::EndMap;
		}

		// --- END ---
		out << YAML::EndMap;
	}


	void Serializer::Serialize(const Ref<Scene>& scene)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << scene->GetName();
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		std::vector<Entity> entites = scene->GetEntities();
		for(const Entity& entity : entites)
		{
			SerializeEntity(out, entity);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(scene->GetFilePath());
		fout << out.c_str();
		fout.close();
	}



	bool Serializer::Deserialize(Ref<Scene>& scene)
	{
		std::string filePath = scene->GetFilePath();
		std::ifstream stream(filePath);
		if (!stream.is_open())
		{
			ENGINE_LOG_ERROR("Failed to open file '{0}'", filePath);
			return false;
		}

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filePath);
		}
		catch (YAML::ParserException e)
		{
			ENGINE_LOG_ERROR("Failed to load .scene file '{0}'\n{1}", filePath, e.what());
			return false;
		}
		if (!data["Scene"]) return false;

		std::string sceneName = data["Scene"].as<std::string>();
		ENGINE_LOG_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (!entities) {
			ENGINE_LOG_WARNING("No entities found in scene file");
			return true;
		}

		for (auto entity : entities)
		{
			uint64_t uuid = entity["Entity"].as<uint64_t>();
			std::string name = entity["Name"].as<std::string>();
			ENGINE_LOG_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

			Entity deserializedEntity = scene->CreateEntity(uuid, name);

			auto transform = entity["Transform"];
			if (transform)
			{
				auto& tc = deserializedEntity.GetComponent<TransformComponent>();
				tc.Position = { transform["Position"][0].as<float>(), transform["Position"][1].as<float>(), transform["Position"][2].as<float>() };
				tc.Rotation = { transform["Rotation"][0].as<float>(), transform["Rotation"][1].as<float>(), transform["Rotation"][2].as<float>() };
				tc.Scale = { transform["Scale"][0].as<float>(), transform["Scale"][1].as<float>(), transform["Scale"][2].as<float>() };
			}

			auto cameraComponent = entity["CameraComponent"];
			if (cameraComponent)
			{
				auto& cc = deserializedEntity.AddComponent<CameraComponent>();

				auto& cameraProps = cameraComponent["Camera"];
				cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

				cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
				cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
				cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

				cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
				cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
				cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

				cc.Main = cameraComponent["Main"].as<bool>();
				cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
			}

			auto spriteRendererComponent = entity["SpriteRendererComponent"];
			if (spriteRendererComponent)
			{
				auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
				std::string materialPath = spriteRendererComponent["Material"].as<std::string>();
				if (materialPath != "null") src.Material = AssetManager::GetAsset<Material>(materialPath);

				std::string texturePath = spriteRendererComponent["Texture"].as<std::string>();
				if (texturePath != "null") src.Texture = AssetManager::GetAsset<Texture2D>(texturePath);
				
				src.Color = { spriteRendererComponent["Color"][0].as<float>(), spriteRendererComponent["Color"][1].as<float>(), spriteRendererComponent["Color"][2].as<float>(), spriteRendererComponent["Color"][3].as<float>() };
				src.TilingFactor = { spriteRendererComponent["TilingFactor"][0].as<float>(), spriteRendererComponent["TilingFactor"][1].as<float>() };
			}

			auto lightComponent = entity["LightComponent"];
			if (lightComponent)
			{
				auto& lc = deserializedEntity.AddComponent<LightComponent>();
				lc.Type = (LightType)lightComponent["Type"].as<int>();
				lc.Color = { lightComponent["Color"][0].as<float>(), lightComponent["Color"][1].as<float>(), lightComponent["Color"][2].as<float>() };
				lc.Intensity = lightComponent["Intensity"].as<float>();
				lc.Range = lightComponent["Range"].as<float>();
				lc.Attenuation.x = lightComponent["Attenuation"][0].as<float>();
				lc.Attenuation.y = lightComponent["Attenuation"][1].as<float>();
				lc.Attenuation.z = lightComponent["Attenuation"][2].as<float>();
				lc.InnerAngle = lightComponent["InnerAngle"].as<float>();
				lc.OuterAngle = lightComponent["OuterAngle"].as<float>();
			}
		}

		return true;
	}
}