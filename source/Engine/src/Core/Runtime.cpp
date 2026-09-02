#include "enginepch.h"
#include "Engine/Core/Runtime.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/Input.h"

namespace engine
{
	void Runtime::Start()
	{

	}

	void Runtime::Stop()
	{
		engine::Audio::StopAll();
		engine::Input::SetCursorMode(engine::Input::CursorMode::Normal); //TODO: handle cursor in editor
	}

	void Runtime::Update()
	{
		if (engine::Input::IsKeyPressedOnce(engine::Key::Escape))
		{
			engine::Input::SetCursorMode(engine::Input::CursorMode::Normal);
		}
	}
}
