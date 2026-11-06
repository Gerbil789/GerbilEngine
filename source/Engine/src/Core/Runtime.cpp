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
		engine::input::SetCursorMode(engine::input::CursorMode::Normal); //TODO: handle cursor in editor
	}

	void Runtime::Update()
	{
		if (engine::input::IsKeyPressed(engine::Key::Escape))
		{
			engine::input::SetCursorMode(engine::input::CursorMode::Normal);
		}
	}
}
