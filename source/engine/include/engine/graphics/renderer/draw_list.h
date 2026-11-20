#pragma once

#include "engine/core/scene.h"
#include "engine/graphics/mesh.h"
#include "engine/graphics/material.h"
#include "engine/core/components.h"
#include <glm/glm.hpp>

namespace engine
{
	struct DrawItem
	{
		engine::MeshHandle mesh;
		engine::MaterialInstanceHandle materialInstance;
		uint32_t subMeshIndex;
		uint32_t indexCount;
		uint32_t firstIndex;
	};

	struct DrawList
	{
		static DrawList Create(const Scene& scene, const component::Camera* camera);

		std::vector<DrawItem> items;
		std::vector<glm::mat4> transforms;

		uint32_t totalDrawItems = 0;
    uint32_t culledCount = 0;
	};
}