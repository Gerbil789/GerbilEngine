#include "enginepch.h"
#include "Engine/Core/Runtime.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/Input.h"

namespace Engine
{
	void Runtime::Start()
	{

	}

	void Runtime::Stop()
	{
		Engine::Audio::StopAll();
		Engine::Input::SetCursorMode(Engine::Input::CursorMode::Normal); //TODO: handle cursor in editor
	}

	void Runtime::Update()
	{
		if (Engine::Input::IsKeyPressedOnce(Engine::Key::Escape))
		{
			Engine::Input::SetCursorMode(Engine::Input::CursorMode::Normal);
		}
	}
}
