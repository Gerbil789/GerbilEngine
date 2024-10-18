#include "enginepch.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/Serializer.h"

namespace Engine
{


	Ref<Asset> MaterialFactory::Load(const std::filesystem::path& path, const std::any& data)
	{
		Ref<Material> material = CreateRef<Material>(path);

		if (!Serializer::Deserialize(material))
		{
			LOG_ERROR("Failed to deserialize material");
		}
		return material;
	}

	Ref<Asset> MaterialFactory::Create(const std::filesystem::path& path, const std::any& data)
	{
		Ref<Material> material = CreateRef<Material>(path);
		return material;
	}
	void Material::SetShader(const Ref<Shader>& shader)
	{
		m_Shader = shader;

		auto& uniformBuffer = m_Shader->GetUniformBuffer();

		for (const auto& element : uniformBuffer)
		{
			if (element.Type == ShaderDataType::Float4)
			{
				//SetProperty(element.Name, glm::vec4(1.0f));
			}


		}
		
	}
}