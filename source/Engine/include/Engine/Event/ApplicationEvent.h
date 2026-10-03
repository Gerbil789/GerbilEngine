#pragma once

#include "Engine/Event/Event.h"
#include "Engine/Core/Scene.h"

namespace engine
{
	struct SceneChangedEvent : public Event
	{
		Scene scene;
		SceneChangedEvent(Scene scene) : scene(scene) {}
	};
}