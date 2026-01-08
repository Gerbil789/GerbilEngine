#include "enginepch.h"
#include "GameInstance.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Graphics/RenderPass/BackgroundPass.h"
#include "Engine/Graphics/RenderPass/OpaquePass.h"
#include "Engine/Graphics/GraphicsContext.h"

namespace Editor
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

		auto entities = scene->GetEntities(true);
		LOG_INFO("GameInstance - Initialized with scene ID: {}, containing {} entities", scene->id, entities.size());

		m_Renderer.SetScene(scene);

		m_ActiveCameraEntity = scene->GetActiveCamera();
		auto& cameraComponent = m_ActiveCameraEntity.GetComponent<Engine::CameraComponent>();
		m_Renderer.SetCamera(cameraComponent.camera);
	}

	void GameInstance::Update()
	{
		if (m_Minimized) return;

		Engine::Camera* camera = m_ActiveCameraEntity.GetComponent<Engine::CameraComponent>().camera;
		camera->SetPosition(m_ActiveCameraEntity.GetComponent<Engine::TransformComponent>().position);
		camera->SetRotation(m_ActiveCameraEntity.GetComponent<Engine::TransformComponent>().rotation);

		wgpu::SurfaceTexture surfaceTexture;
		m_GameWindow->GetSurface().getCurrentTexture(&surfaceTexture);
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

		m_GameWindow->GetSurface().present();
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
		OnExit();
	}

	void GameInstance::OnWindowResize(Engine::WindowResizeEvent& e)
	{
		m_Minimized = (e.GetWidth() == 0 || e.GetHeight() == 0) ? true : false;

		m_Renderer.Resize((uint32_t)e.GetWidth(), (uint32_t)e.GetHeight());
	}

}