#pragma once

#include "Engine/Event/Event.h"
#include "Engine/Core/UUID.h"
#include "Editor/Core/SelectionManager.h"

namespace Editor
{
	struct SelectionChangedEvent : public Engine::Event
	{
		SelectionContext context;
		Engine::Uuid id;

		SelectionChangedEvent(SelectionContext context, Engine::Uuid id) : context(context), id(id) {}
	};

	struct FocusAssetEvent : public Engine::Event
	{
		Engine::Uuid id;

		FocusAssetEvent(const Engine::Uuid& id) : id(id) {}
	};

	struct FocusEntityEvent : public Engine::Event
	{
		Engine::Uuid id;

		FocusEntityEvent(const Engine::Uuid& id) : id(id) {}
	};
}