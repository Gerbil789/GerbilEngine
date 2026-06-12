#include "SelectionManager.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"

namespace Editor
{
  void SelectionGroup::Select(Engine::Uuid id, bool additive)
  {
    if (!id) return;

    if (std::find(m_Selection.begin(), m_Selection.end(), id) == m_Selection.end())
    {
      if (!additive)
      {
        m_Selection.clear();
      }

      m_Selection.push_back(id);

      Engine::EventBus::Get().Publish(SelectionChangedEvent{ m_Context, id });
    }
  }

  void SelectionGroup::Toggle(Engine::Uuid id)
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

    Engine::EventBus::Get().Publish(SelectionChangedEvent{ m_Context, id });
  }

  void SelectionGroup::Clear()
  {
    if (m_Selection.empty()) return;
    m_Selection.clear();
    Engine::EventBus::Get().Publish(SelectionChangedEvent{ m_Context, 0 });
  }

  bool SelectionGroup::IsSelected(Engine::Uuid id) const
  {
    return std::find(m_Selection.begin(), m_Selection.end(), id) != m_Selection.end();
  }

  Engine::Uuid SelectionGroup::GetPrimary() const
  {
    return m_Selection.empty() ? Engine::Uuid{} : m_Selection.back();
  }

  const std::vector<Engine::Uuid>& SelectionGroup::GetAll() const
  {
    return m_Selection;
  }
}