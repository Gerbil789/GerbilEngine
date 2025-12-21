#include "enginepch.h"
#include "AssetSelection.h"

namespace Editor
{
	void AssetSelection::Clear()
	{
		m_Selected.clear();
	}

	void AssetSelection::Select(Engine::UUID id)
	{
		m_Selected.clear();
		m_Selected.push_back(id);
	}

	bool AssetSelection::IsSelected(Engine::UUID id) const
	{
		return std::find(m_Selected.begin(), m_Selected.end(), id) != m_Selected.end();
	}

	Engine::UUID AssetSelection::GetPrimary() const
	{
		if (m_Selected.empty())
		{
			return Engine::UUID(0);
		}

		return m_Selected[0];
	}
}