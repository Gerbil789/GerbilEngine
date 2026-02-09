#include "EntitySelection.h"
#include "AssetSelection.h"

namespace Editor
{
	void EntitySelection::Clear()
	{
		m_Selected.clear();
	}

	void EntitySelection::Select(Engine::Entity entity, bool additive)
	{
		if (!additive)
		{
			m_Selected.clear();
		}

		if(IsSelected(entity))
		{
			return;
		}

		m_Selected.push_back(entity);
	}

	bool EntitySelection::IsSelected(Engine::Entity entity) const
	{
		for (const auto& e : m_Selected)
		{
			if (e == entity) return true;
		}
		return false;

	}

	Engine::Entity EntitySelection::GetPrimary() const
	{
		if (m_Selected.empty())
		{
			return Engine::Entity::Null();
		}

		// return the last selected entity as primary
		return m_Selected.back();
	}

	const std::vector<Engine::Entity>& EntitySelection::GetAll() const
	{
		return m_Selected;
	}
}