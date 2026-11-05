#pragma once

#include "Engine/Event/Event.h"
#include "Engine/Core/Scene.h"

namespace engine
{
	struct SceneChangedEvent : public Event
	{
		SceneHandle scene;
		SceneChangedEvent(SceneHandle scene) : scene(scene) {}
	};
}