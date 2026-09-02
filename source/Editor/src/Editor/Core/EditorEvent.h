#pragma once

#include "Engine/Event/Event.h"
#include "Engine/Core/UUID.h"
#include "Editor/Core/SelectionManager.h"

namespace editor
{
	struct SelectionChangedEvent : public engine::Event
	{
		SelectionContext context;
		engine::Uuid id;

		SelectionChangedEvent(SelectionContext context, engine::Uuid id) : context(context), id(id) {}
	};

	struct FocusAssetEvent : public engine::Event
	{
		engine::Uuid id;

		FocusAssetEvent(const engine::Uuid& id) : id(id) {}
	};

	struct FocusEntityEvent : public engine::Event
	{
		engine::Uuid id;

		FocusEntityEvent(const engine::Uuid& id) : id(id) {}
	};
}