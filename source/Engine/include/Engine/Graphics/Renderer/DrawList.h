#pragma once

#include "Engine/Core/UUID.h"
#include <glm/glm.hpp>

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
		Engine::Uuid entityId; //TODO: remove this? its used only for editor picker
	};

	class DrawList
	{
	public:
		static DrawList CreateFromScene(Scene& scene);

		const std::vector<DrawItem>& GetItems() const { return m_Items; }
		const std::vector<glm::mat4>& GetTransforms() const { return m_Transforms; }

		auto size() const { return m_Items.size(); }
		bool empty() const { return m_Items.empty(); }

	private:
		std::vector<DrawItem> m_Items;
		std::vector<glm::mat4> m_Transforms;
	};
}