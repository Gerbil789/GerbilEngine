#pragma once

#include "Engine/Core/UUID.h"
#include <entt.hpp>

namespace Engine
{
	class Scene;

	struct DrawItem
	{
		Engine::Uuid meshId;
		Engine::Uuid materialId;
		uint32_t subMeshIndex;
		uint32_t indexCount;
		uint32_t firstIndex;
		entt::entity entity;
	};

	class DrawList
	{
	public:
		static DrawList CreateFromScene(Scene& scene);

		auto begin() { return items.begin(); }
		auto end() { return items.end(); }

		auto begin() const { return items.begin(); }
		auto end()   const { return items.end(); }

		auto size() const { return items.size(); }

	private:
		std::vector<DrawItem> items;
	};
}