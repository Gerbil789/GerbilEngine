#include "enginepch.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/Serializer.h"

namespace Engine
{
	Ref<Asset> MaterialFactory::Load(const std::filesystem::path& path, const std::any& data)
	{
		Ref<Material> material = CreateRef<Material>(path);
		Serializer::Deserialize(material);
		return material;
	}

	Ref<Asset> MaterialFactory::Create(const std::filesystem::path& path, const std::any& data)
	{
		Ref<Material> material = CreateRef<Material>(path);
		material->SetShader(AssetManager::GetAsset<Shader>("resources/shaders/testshader.wgsl")); //TODO: use diffrent default shader
		Serializer::Serialize(material);
		return material;
	}

	void Material::SetShader(const Ref<Shader>& shader)
	{
		m_Shader = shader;
		//m_UniformBuffer = CreateRef<UniformBuffer>(shader->GetMaterialBufferLayout().size(), 1);
		//this->SetProperties();
	}

	/*void Material::SetProperties()
	{
		this->ClearProperties();

		const BufferLayout& properties = m_Shader->GetMaterialBufferLayout();
		for (const auto& prop : properties)
		{
			switch (prop.Type)
			{
			case ShaderDataType::Int:
				SetProperty(prop.Name, 0);
				break;
			case ShaderDataType::Float:
				SetProperty(prop.Name, 0.0f);
				break;
			case ShaderDataType::Float2:
				SetProperty(prop.Name, glm::vec2(0.0f));
				break;
			case ShaderDataType::Float3:
				SetProperty(prop.Name, glm::vec3(0.0f));
				break;
			case ShaderDataType::Float4:
				SetProperty(prop.Name, glm::vec4(0.0f));
				break;
			
			default:
				LOG_WARNING("Unknown ShaderDataType (Material.cpp)");
				break;
			}

		}
	}*/
}