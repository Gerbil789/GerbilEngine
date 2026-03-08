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

using namespace Engine;

void GameManager::OnStart()
{
	auto scene = SceneManager::GetActiveScene();


	for(int x = 0; x < 5; x++)
	{
		for(int z = 0; z < 5; z++)
		{
			auto sphere = scene->CreateEntity("Sphere");
			sphere.GetComponent<TransformComponent>().position = { static_cast<float>(x) * 2.0f, 0.0f, static_cast<float>(z) * 2.0f };
			sphere.AddComponent<MeshComponent>(m_Mesh);
			
		}
	}


}

void GameManager::OnUpdate()
{
	
}
