#include "enginepch.h"
#include "Serializer.h"
#include "Engine/Utils/YAMLSerializer.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Renderer/Material.h"

namespace Engine::Serializer
{
	constexpr const char* SHADER_KEY = "Shader";
	constexpr const char* PROPERTIES_KEY = "Properties";

	void Serialize(const Ref<Material>& material)
	{
		if (!material)
		{
			LOG_ERROR("Material is null, cannot serialize.");
			return;
		}

		//YAML::Emitter out;
		//out << YAML::BeginMap;

		//auto shader = material->GetShader();
		////writer.Write(SHADER_KEY, (shader ? shader->GetFilePath().string() : ""));

		//out << YAML::Key << PROPERTIES_KEY << YAML::Value << YAML::BeginMap;

		//auto properties = shader->GetUniformBuffer();
		//for (auto& element : properties)
		//{
		//	std::string name = element.Name;

		//	if (!material->HasProperty(name))
		//	{
		//		LOG_ERROR("Material does not have property: {0}", name);
		//		continue;
		//	}

		//	auto type = element.Type;

		//	if (type == ShaderDataType::Float4)
		//	{
		//		WriteVec4(out, name, material->GetProperty<glm::vec4>(name));
		//	}
		//	else if (type == ShaderDataType::Float3)
		//	{
		//		WriteVec3(out, name, material->GetProperty<glm::vec3>(name));
		//	}
		//	else if (type == ShaderDataType::Float2)
		//	{
		//		WriteVec2(out, name, material->GetProperty<glm::vec2>(name));
		//	}
		//	else if (type == ShaderDataType::Float)
		//	{
		//		WriteFloat(out, name, material->GetProperty<float>(name));
		//	}
		//	else if (type == ShaderDataType::Int)
		//	{
		//		WriteInt(out, name, material->GetProperty<int>(name));
		//	}
		//	//else if (type == ShaderDataType::Texture2D)
		//	//{
		//	//	//writeAssetPath(name, material->GetProperty<Ref<Texture2D>>(name));
		//	//}
		//	else
		//	{
		//		LOG_ERROR("Unknown material property type!");
		//	}
		//
		//}

		//auto properties = material->GetProperties();
		/*for (auto& property : properties)
		{
			std::string name = property.first;
			auto value = property.second;

			if (std::holds_alternative<glm::vec4>(value))
			{
				WriteVec4(out, name, std::get<glm::vec4>(value));
			}
			else if (std::holds_alternative<glm::vec3>(value))
			{
				WriteVec3(out, name, std::get<glm::vec3>(value));
			}
			else if (std::holds_alternative<glm::vec2>(value))
			{
				WriteVec2(out, name, std::get<glm::vec2>(value));
			}
			else if (std::holds_alternative<float>(value))
			{
				WriteFloat(out, name, std::get<float>(value));
			}
			else if (std::holds_alternative<int>(value))
			{
				WriteInt(out, name, std::get<int>(value));
			}
			else if (std::holds_alternative<Ref<Texture2D>>(value))
			{
				writeAssetPath(name, std::get<Ref<Texture2D>>(value));
			}
			else
			{
				LOG_ERROR("Unknown material property type!");
			}
		}*/

		//out << YAML::EndMap; //properties
		//out << YAML::EndMap;

		//std::ofstream fout(material->GetFilePath());
		//if (!fout.is_open())
		//{
		//	LOG_ERROR("Failed to open file for serialization: {0}", material->GetFilePath());
		//	return;
		//}
		////fout << out.c_str();
		//fout.close();
	}

	bool Serializer::Deserialize(Ref<Material>& material)
	{
		std::filesystem::path path = material->GetFilePath();
		std::ifstream stream(path);
		if (!stream.is_open())
		{
			LOG_ERROR("Failed to open file {0}", path);
			return false;
		}

		/*YAML::Node data = YAML::LoadFile(path.string());
		if (!data[SHADER_KEY].IsDefined())
		{
			LOG_ERROR("Shader field missing in material! {0}", path);
			return false;
		}

		auto loadTexture = [&](const std::string& key) -> Ref<Texture2D> {
			if (data[key].IsNull()) { return nullptr; }
			return AssetManager::Get<Texture2D>(data[key].as<std::string>());
			};*/

		//auto shader = AssetManager::GetAsset<Shader>(data[SHADER_KEY].as<std::string>());
		//auto shader = CreateRef<Shader>();
		//material->SetShader(shader);

		//auto materialProperties = shader->GetMaterialBufferLayout();

		//auto shaderProperties = shader->GetUniformBuffers(); //TODO: check if properties of material and shader match
		//auto materialProperties = data[PROPERTIES_KEY];

		//for (auto& property : materialProperties)
		//{
		//	std::string name = property.Name;
		//	auto type = property.Type;

		//	if (!data[PROPERTIES_KEY][name].IsDefined())
		//	{
		//		continue;
		//	}

		//	if (type == ShaderDataType::Float4)
		//	{
		//		material->SetProperty(name, ReadVec4(data[PROPERTIES_KEY], name));
		//	}
		//	else if (type == ShaderDataType::Float3)
		//	{
		//		material->SetProperty(name, ReadVec3(data[PROPERTIES_KEY], name));
		//	}
		//	else if (type == ShaderDataType::Float2)
		//	{
		//		material->SetProperty(name, ReadVec2(data[PROPERTIES_KEY], name));
		//	}
		//	else if (type == ShaderDataType::Float)
		//	{
		//		material->SetProperty(name, ReadFloat(data[PROPERTIES_KEY], name));
		//	}
		//	else if (type == ShaderDataType::Int)
		//	{
		//		material->SetProperty(name, ReadInt(data[PROPERTIES_KEY], name));
		//	}
		//	//else if (type == ShaderDataType::Texture2D)
		//	//{
		//	//	//writeAssetPath(name, material->GetProperty<Ref<Texture2D>>(name));
		//	//}
		//	else
		//	{
		//		LOG_ERROR("Unknown material property type!");
		//	}
		//	
		//}

		material->SetModified(false);
		return true;
	}

	//void SerializeTransformComponent(YAML::Emitter& out, const TransformComponent& tc)
	//{
	//	out << YAML::Key << "Transform";
	//	out << YAML::BeginMap;
	//	out << YAML::Key << "Position" << YAML::Flow << YAML::BeginSeq << tc.Position.x << tc.Position.y << tc.Position.z << YAML::EndSeq;
	//	out << YAML::Key << "Rotation" << YAML::Flow << YAML::BeginSeq << tc.Rotation.x << tc.Rotation.y << tc.Rotation.z << YAML::EndSeq;
	//	out << YAML::Key << "Scale" << YAML::Flow << YAML::BeginSeq << tc.Scale.x << tc.Scale.y << tc.Scale.z << YAML::EndSeq;
	//	out << YAML::EndMap;
	//}

	//void SerializeMeshComponent(YAML::Emitter& out, const MeshComponent& mc)
	//{
	//	out << YAML::Key << "MeshComponent";
	//	out << YAML::BeginMap;
	//	out << YAML::Key << "Mesh" << YAML::Value << (mc.Mesh ? mc.Mesh->GetFilePath().string() : "null");
	//	out << YAML::Key << "Material" << YAML::Value << (mc.Material ? mc.Material->GetFilePath().string() : "null");
	//	out << YAML::EndMap;
	//}

	//void SerializeHierarchyComponent(YAML::Emitter& out, const HierarchyComponent& hc)
	//{
	//	//out << YAML::Key << "HierarchyComponent";
	//	//out << YAML::BeginMap;
	//	//out << YAML::Key << "Parent" << YAML::Value << (hc.Parent != entt::null ? std::to_string(hc.Parent) : "null");
	//	//out << YAML::Key << "FirstChild" << YAML::Value << (hc.FirstChild != entt::null ? std::to_string(hc.FirstChild) : "null");
	//	//out << YAML::Key << "NextSibling" << YAML::Value << (hc.NextSibling != entt::null ? std::to_string(hc.NextSibling) : "null");
	//	//out << YAML::Key << "PrevSibling" << YAML::Value << (hc.PrevSibling != entt::null ? std::to_string(hc.PrevSibling) : "null");
	//	//out << YAML::EndMap;
	//}

	//void SerializeCameraComponent(YAML::Emitter& out, const CameraComponent& cc)
	//{
	//	//out << YAML::Key << "CameraComponent";
	//	//out << YAML::BeginMap;
	//	//out << YAML::Key << "Main" << YAML::Value << cc.Main;
	//	//out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.FixedAspectRatio;
	//	//const auto& projectionData = cc.Camera.GetProjectionData();
	//	//out << YAML::Key << "ProjectionType" << YAML::Value << (int)projectionData.Type;
	//	//out << YAML::Key << "PerspectiveFOV" << YAML::Value << projectionData.Perspective.FOV;
	//	//out << YAML::Key << "PerspectiveNear" << YAML::Value << projectionData.Perspective.Near;
	//	//out << YAML::Key << "PerspectiveFar" << YAML::Value << projectionData.Perspective.Far;
	//	//out << YAML::Key << "OrthographicSize" << YAML::Value << projectionData.Orthographic.Size;
	//	//out << YAML::Key << "OrthographicNear" << YAML::Value << projectionData.Orthographic.Near;
	//	//out << YAML::Key << "OrthographicFar" << YAML::Value << projectionData.Orthographic.Far;
	//	//out << YAML::EndMap;
	//}

	//void SerializeLightComponent(YAML::Emitter& out, const LightComponent& lc)
	//{
	//	//out << YAML::Key << "LightComponent";
	//	//out << YAML::BeginMap;
	//	//out << YAML::Key << "Type" << YAML::Value << (int)lc.Type;
	//	//out << YAML::Key << "Color" << YAML::Flow << YAML::BeginSeq << lc.Color.r << lc.Color.g << lc.Color.b << YAML::EndSeq;
	//	//out << YAML::Key << "Intensity" << YAML::Value << lc.Intensity;
	//	//out << YAML::Key << "Range" << YAML::Value << lc.Range;
	//	//out << YAML::Key << "Attenuation" << YAML::Flow << YAML::BeginSeq 
	//	//	<< lc.Attenuation.x << lc.Attenuation.y << lc.Attenuation.z 
	//	//	<< YAML::EndSeq;
	//	//out << YAML::Key << "InnerAngle" << YAML::Value << lc.InnerAngle;
	//	//out << YAML::Key << "OuterAngle" << YAML::Value << lc.OuterAngle;
	//	//out << YAML::EndMap;
	//}

	//void SerializeEntity(YAML::Emitter& out, const Entity& entity)
	//{


	void Serializer::Serialize(Scene* scene)
	{
		YamlSerializer yaml;
		yaml.BeginMap();
		yaml.Write("Scene", scene->GetFilePath().string());

		// Entities
		yaml.BeginSequence("Entities");
		for (const Entity& entity : scene->GetEntities())
		{
			yaml.BeginMap();
			yaml.Write("Entity", entity.GetComponent<NameComponent>().Name);
			yaml.Write("ID", fmt::format("{:08X}", (uint32_t)entity.GetComponent<IdentityComponent>().ID)); //Hexadecimal format for UUID
			yaml.Write("Enabled", entity.GetComponent<IdentityComponent>().Enabled);

			yaml.BeginMap("Transform");
			const auto& transform = entity.GetComponent<TransformComponent>();
			yaml.Write("Position", transform.Position);
			yaml.Write("Rotation", transform.Rotation);
			yaml.Write("Scale", transform.Scale);
			yaml.EndMap();

			yaml.BeginSequence("Components");

			// MeshComponent
			yaml.BeginMap("MeshComponent");
			const auto& meshComponent = entity.GetComponent<MeshComponent>();
			yaml.Write("Mesh", meshComponent.Mesh ? meshComponent.Mesh->GetFilePath().string() : "null");
			yaml.Write("Material", meshComponent.Material ? meshComponent.Material->GetFilePath().string() : "null");
			yaml.EndMap(); 

			yaml.EndSequence(); // Components
			yaml.EndMap(); // Entity
		}

		yaml.EndSequence(); // Entities

		yaml.EndMap();
		yaml.SaveToFile(scene->GetFilePath());
	}



	bool Serializer::Deserialize(Scene* scene)
	{
	
		return true;
	}
}