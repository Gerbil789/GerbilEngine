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
		std::vector<DrawItem> items;

		static DrawList CreateFromScene(Scene* scene);
	};
}