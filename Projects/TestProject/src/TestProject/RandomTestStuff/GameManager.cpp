#include "GameManager.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Log.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scene/Components.h"
#include "Engine/Audio/AudioClip.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/Time.h"
#include "Engine/Utility/Color.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Core/Resources.h"
#include "Engine/Asset/AssetRegistry.h"
#include <format>
#include "Engine/Asset/Serializer/MaterialSerializer.h"
#include "Engine/Graphics/Texture/Texture2D.h"

using namespace Engine;

void GameManager::OnStart()
{
	Scene& scene = SceneManager::GetActiveScene();
	auto& registry = scene.GetRegistry();
	int i = 1;

	for(int x = 0; x < 5; x++)
	{
		for(int z = 0; z < 5; z++)
		{
			auto sphere = scene.CreateEntity(std::format("Sphere_{}", i));

			registry.get<TransformComponent>(sphere).position = { static_cast<float>(x) * 3.0f, 0.0f, static_cast<float>(z) * 3.0f };

			std::string path = std::format("Materials/tmp/tmp_{}.mat", i);
			Material& mat = AssetManager::CreateAsset<Material>(path);

			mat.SetShader(m_Shader);

			mat.SetParameter("albedo", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
			mat.SetParameter("tiling", glm::vec2{ 1.0f, 1.0f });
			mat.SetTexture("NormalTexture", RESOURCES::TEXTURE::NORMAL);

			float roughness = static_cast<float>(x) / 4.0f;
			float metallic = static_cast<float>(z) / 4.0f;

			mat.SetParameter("roughness", roughness);
			mat.SetParameter("metallic", metallic);

			//MaterialSerializer::Serialize(mat, path);

			auto& mc = registry.emplace<MeshComponent>(sphere, RESOURCES::MESH::SPHERE);
			mc.materials = { mat.id };

			i++;
		}
	}
}

void GameManager::OnUpdate()
{
	
}
