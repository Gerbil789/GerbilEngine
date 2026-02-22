#include "enginepch.h"
#include "Engine/Core/GameInstance.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Graphics/RenderPass/BackgroundPass.h"
#include "Engine/Graphics/RenderPass/OpaquePass.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Components.h"
#include "Engine/Script/Script.h"

namespace Engine
{
	GameInstance::GameInstance()
	{
		LOG_INFO("========== Starting game instance ==========");

		Engine::WindowSpecification windowSpec;
		windowSpec.title = "Game";
		windowSpec.width = 1600 / 2;
		windowSpec.height = 900 / 2;
		windowSpec.iconPath = "Resources/Engine/icons/logo.png";

		m_GameWindow = new Engine::Window(windowSpec);
		m_GameWindow->SetEventCallback([this](Engine::Event& e) {this->OnEvent(e); });



		m_Renderer.AddPass(new Engine::BackgroundPass());
		m_Renderer.AddPass(new Engine::OpaquePass());

		m_Renderer.Resize(windowSpec.width, windowSpec.height);
	}

	GameInstance::~GameInstance()
	{
		delete m_GameWindow;
	}

	//TODO: copy scene (deep copy), not reference it
	void GameInstance::Initialize(Engine::Scene* scene)
	{
		m_Running = true;
		m_ActiveScene = Scene::Copy(scene);

		auto entities = m_ActiveScene->GetEntities(true);
		LOG_INFO("GameInstance - Initialized with scene ID: {}, containing {} entities", m_ActiveScene->id, entities.size());

		m_Renderer.SetScene(m_ActiveScene);

		m_ActiveCameraEntity = m_ActiveScene->GetActiveCamera();
		auto& cameraComponent = m_ActiveCameraEntity.GetComponent<Engine::CameraComponent>();
		m_Renderer.SetCamera(cameraComponent.camera);
	}

	void GameInstance::Update()
	{
		if (m_GameWindow->IsMinimized()) return;


		// update scripts
		for (auto& ent : m_ActiveScene->GetEntities<Engine::ScriptComponent>())
		{
			auto& scriptComp = ent.GetComponent<Engine::ScriptComponent>();
			if (scriptComp.instance)
			{
				scriptComp.instance->OnUpdate();
			}
		}


		Engine::Camera* camera = m_ActiveCameraEntity.GetComponent<Engine::CameraComponent>().camera;
		camera->SetPosition(m_ActiveCameraEntity.GetComponent<Engine::TransformComponent>().position);
		camera->SetRotation(m_ActiveCameraEntity.GetComponent<Engine::TransformComponent>().rotation);

		wgpu::SurfaceTexture surfaceTexture;

		wgpu::Surface& surface = *static_cast<wgpu::Surface*>(m_GameWindow->GetSurface()); //TODO: store
		surface.getCurrentTexture(&surfaceTexture);

		if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal)
		{
			LOG_ERROR("Failed to get current surface texture. status: {}", (int)surfaceTexture.status);
			return;
		}

		wgpu::Texture texture = surfaceTexture.texture;
		wgpu::TextureView targetView = texture.createView();

		if (!targetView)
		{
			LOG_ERROR("Failed to create texture view for surface texture");
			return;
		}

		m_Renderer.SetColorTarget(targetView);

		m_Renderer.RenderScene();

		surface.present();
	}

	void GameInstance::OnEvent(Engine::Event& event)
	{
		Engine::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<Engine::WindowCloseEvent>([this](auto&) {Close(); });
		dispatcher.Dispatch<Engine::WindowResizeEvent>([this](auto& e) {OnWindowResize(e); });
	}

	void GameInstance::Close()
	{
		LOG_INFO("========== Closing game instance ===========");
		m_Running = false;
		delete m_ActiveScene;
		m_ActiveScene = nullptr;
		OnExit();
	}

	void GameInstance::OnWindowResize(Engine::WindowResizeEvent& e)
	{
		m_Renderer.Resize((uint32_t)e.GetWidth(), (uint32_t)e.GetHeight());
	}

}