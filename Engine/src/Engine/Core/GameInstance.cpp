#include "enginepch.h"
#include "GameInstance.h"

#include "Engine/Scene/Entity.h"

namespace Engine
{
	GameInstance::GameInstance()
	{
		LOG_INFO("========== Starting game instance ==========");

		WindowSpecification windowSpec;
		windowSpec.title = "Game";
		windowSpec.width = 1600;
		windowSpec.height = 900;
		windowSpec.iconPath = "Resources/Engine/icons/logo.png";

		m_GameWindow = new Window(windowSpec);
		m_GameWindow->SetEventCallback([this](Event& e) {this->OnEvent(e); });
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
	}

	void GameInstance::Update()
	{

	}

	void GameInstance::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>([this](auto&) {Close(); });
		dispatcher.Dispatch<WindowResizeEvent>([this](auto& e) {OnWindowResize(e); });
	}

	void GameInstance::Close()
	{
		LOG_INFO("========== Closing game instance ==========");
		m_Running = false;
		OnExit();
	}

	void GameInstance::OnWindowResize(WindowResizeEvent& e)
	{
		m_Minimized = (e.GetWidth() == 0 || e.GetHeight() == 0) ? true : false;
	}

}