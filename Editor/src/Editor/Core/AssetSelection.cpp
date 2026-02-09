#include "AssetSelection.h"

namespace Editor
{
	void AssetSelection::Clear()
	{
		m_Selected.clear();
	}

	void AssetSelection::Select(Engine::Uuid id)
	{
		m_Selected.clear();
		m_Selected.push_back(id);
	}

	bool AssetSelection::IsSelected(Engine::Uuid id) const
	{
		return std::find(m_Selected.begin(), m_Selected.end(), id) != m_Selected.end();
	}

	Engine::Uuid AssetSelection::GetPrimary() const
	{
		if (m_Selected.empty())
		{
			return Engine::Uuid(0);
		}

		return m_Selected[0];
	}
}