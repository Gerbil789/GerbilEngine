#pragma once

#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include <glm/glm.hpp>
#include <entt/entity/registry.hpp>

namespace engine
{
	struct DrawItem
	{
		engine::Mesh mesh;
		engine::MaterialInstance materialInstance;
		uint32_t subMeshIndex;
		uint32_t indexCount;
		uint32_t firstIndex;
	};

	struct DrawList
	{
		static DrawList Create(const entt::registry& registry);

		std::vector<DrawItem> items;
		std::vector<glm::mat4> transforms;
	};
}