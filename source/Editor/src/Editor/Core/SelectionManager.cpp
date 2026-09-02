#include "SelectionManager.h"
#include "Engine/Event/EventBus.h"
#include "Engine/Event/ApplicationEvent.h"
#include "Editor/Core/EditorEvent.h"

namespace editor
{
	void SelectionGroup::Select(engine::Uuid id, bool additive)
	{
		if (!id) return;

		if (!additive)
		{
			m_Selection.clear();
		}

		m_Selection.push_back(id);

		engine::EventBus::Publish(SelectionChangedEvent{ m_Context, id });
	}

	void SelectionGroup::Toggle(engine::Uuid id)
	{
		auto it = std::find(m_Selection.begin(), m_Selection.end(), id);
		if (it != m_Selection.end())
		{
			m_Selection.erase(it);
		}
		else if (id)
		{
			m_Selection.push_back(id);
		}

		engine::EventBus::Publish(SelectionChangedEvent{ m_Context, id });
	}

	void SelectionGroup::Clear()
	{
		if (m_Selection.empty()) return;
		m_Selection.clear();
		engine::EventBus::Publish(SelectionChangedEvent{ m_Context, 0 });
	}

	bool SelectionGroup::IsSelected(engine::Uuid id) const
	{
		return std::find(m_Selection.begin(), m_Selection.end(), id) != m_Selection.end();
	}

	engine::Uuid SelectionGroup::GetPrimary() const
	{
		return m_Selection.empty() ? engine::Uuid{} : m_Selection.back();
	}

	const std::vector<engine::Uuid>& SelectionGroup::GetAll() const
	{
		return m_Selection;
	}


	void SelectionManager::Initialize()
	{
		engine::EventBus::Subscribe<engine::SceneChangedEvent>([](const engine::SceneChangedEvent&)
			{
				Entities.Clear();
				return false;
			});
	}
}