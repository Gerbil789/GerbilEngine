#pragma once

#include "Engine/Event/Event.h"
#include "Engine/Core/UUID.h"
#include <entt/entity/entity.hpp>

namespace editor
{
	struct SelectAssetEvent : public engine::Event
	{
		engine::Uuid id;
		SelectAssetEvent(engine::Uuid id) : id(id) {}
	};

	struct SelectEntityEvent : public engine::Event
	{
		entt::entity entity;
		SelectEntityEvent(entt::entity entity) : entity(entity) {}
	};

	struct FocusEntityEvent : public engine::Event
	{
		entt::entity entity;
		FocusEntityEvent(entt::entity entity) : entity(entity) {}
	};
}