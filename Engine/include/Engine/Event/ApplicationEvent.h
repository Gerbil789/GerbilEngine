#pragma once

#include "Engine/Event/Event.h"
#include "Engine/Core/UUID.h"

namespace Engine
{
	struct SceneChangedEvent : public Event
	{
		Uuid id;

		SceneChangedEvent(Uuid id) : id(id) {}
	};
}