#include "enginepch.h"
#include "EditorApp.h"
#include "Editor/Core/EditorSceneController.h"
#include "Editor/Core/EditorWindowManager.h"

//tmp
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Renderer/Shaders/PhongShader.h"
#include "Engine/Renderer/Shaders/FlatColorShader.h"

namespace Editor
{
	EditorApp::EditorApp(std::filesystem::path projectPath) : Application("Gerbil Editor - " + projectPath.lexically_normal().filename().string())
	{
		ENGINE_PROFILE_FUNCTION();
		m_Project = Engine::Project(projectPath);

		LOG_TRACE("Starting Gerbil Editor for project: {0}", m_Project.GetTitle());

		EditorWindowManager::Initialize();

		Engine::SceneManager::CreateScene("MyScene"); //TODO: load default scene from project if there is one

		{
			auto scene = Engine::SceneManager::GetActiveScene();

			auto texture = Engine::AssetManager::Get<Engine::Texture2D>(EDITOR_RESOURCES / "icons/skull.png");

			// Entity 1
			{
				auto cube = scene->CreateEntity("TextureCube");
				auto mesh = Engine::AssetManager::Get<Engine::Mesh>(ENGINE_RESOURCES / "models/cube.glb");
				auto material = Engine::AssetManager::Create<Engine::Material>(ENGINE_RESOURCES / "materials/red.material");
				material->SetTexture("AlbedoTexture", texture);
				material->SetShader(CreateRef<Engine::PhongShader>());
				material->SetValue("Color", glm::vec4(0.8f, 0.1f, 0.2f, 1.0f));

				auto& component = cube.AddComponent<Engine::MeshComponent>();
				component.Material = material;
				component.Mesh = mesh;

				cube.GetComponent<Engine::TransformComponent>().Position = { 2.0f, 0.0f, 0.0f };
				cube.GetComponent<Engine::TransformComponent>().Rotation = { 45.0f, 45.0f, 0.0f };

				LOG_WARNING("Created entity '{0}' with ID: {1}", cube.GetName(), (uint32_t)cube.GetUUID());
			}

			// Entity 2
			{
				auto cube = scene->CreateEntity("BlueCube");
				auto mesh = Engine::AssetManager::Get<Engine::Mesh>(ENGINE_RESOURCES / "models/cube.glb");
				auto material = Engine::AssetManager::Create<Engine::Material>(ENGINE_RESOURCES / "materials/blue.material");
				material->SetShader(CreateRef<Engine::FlatColorShader>());
				material->SetValue("Color", glm::vec4(0.2f, 0.1f, 0.8f, 1.0f));
				auto& component = cube.AddComponent<Engine::MeshComponent>();
				component.Material = material;
				component.Mesh = mesh;

				cube.GetComponent<Engine::TransformComponent>().Position = { -2.0f, 0.0f, 0.0f };
				cube.GetComponent<Engine::TransformComponent>().Rotation = { 45.0f, 45.0f, 0.0f };

				LOG_WARNING("Created entity '{0}' with ID: {1}", cube.GetName(), (uint32_t)cube.GetUUID());
			}
		}
	}

	EditorApp::~EditorApp()
	{
		ENGINE_PROFILE_FUNCTION();
		EditorWindowManager::Shutdown();
	}

	void EditorApp::Run()
	{
		ENGINE_PROFILE_FUNCTION();
		while (m_Running)
		{
			ENGINE_PROFILE_SCOPE("RunLoop");

			//TODO: pack this fps counter into something, dont bloat main loop with it
			// -----------------------------------
			auto now = std::chrono::steady_clock::now();
			float deltaTime = std::chrono::duration<float>(now - m_LastFrameTime).count();
			m_LastFrameTime = now;
			m_FPSCounter.Update(deltaTime);
			Engine::Timestep ts(deltaTime);
			// -----------------------------------

			if (m_Minimized) continue;

			EditorWindowManager::OnUpdate(ts);
			m_Window->OnUpdate();
		}
	}

	void EditorApp::OnEvent(Engine::Event& e)
	{
		ENGINE_PROFILE_FUNCTION();
		Application::OnEvent(e);
		EditorSceneController::OnEvent(e);
		EditorWindowManager::OnEvent(e);
	}
}