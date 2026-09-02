#pragma once

#include "Engine/Asset/AssetHandle.h"
#include <glm/glm.hpp>

namespace engine
{
	class SceneAsset;

	struct DrawItem
	{
		engine::Mesh mesh;
		engine::Material material;
		uint32_t subMeshIndex;
		uint32_t indexCount;
		uint32_t firstIndex;
		engine::Uuid entityId; //TODO: remove this? its used only for editor picker
	};

	class DrawList
	{
	public:
		static DrawList CreateFromScene(SceneAsset& scene);

		const std::vector<DrawItem>& GetItems() const { return m_Items; }
		const std::vector<glm::mat4>& GetTransforms() const { return m_Transforms; }

		auto size() const { return m_Items.size(); }
		bool empty() const { return m_Items.empty(); }

	private:
		std::vector<DrawItem> m_Items;
		std::vector<glm::mat4> m_Transforms;
	};
}