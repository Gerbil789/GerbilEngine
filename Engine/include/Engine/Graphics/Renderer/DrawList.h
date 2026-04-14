#pragma once

#include "Engine/Scene/Entity.h"

namespace Engine
{
	class Scene;
	class Mesh;
	struct SubMesh;

	struct DrawItem
	{
		Entity entity;
		Mesh* mesh;
		const SubMesh* subMesh;
		uint32_t modelIndex; // index into model buffer
	};

	class DrawList
	{
	public:
		static DrawList CreateFromScene(Scene* scene);

		// Non-const iterators
		auto begin() { return items.begin(); }
		auto end() { return items.end(); }

		// Const iterators
		auto begin() const { return items.begin(); }
		auto end()   const { return items.end(); }

		auto size() const { return items.size(); }

	private:
		std::vector<DrawItem> items;
	};
}