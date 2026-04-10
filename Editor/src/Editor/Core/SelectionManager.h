#pragma once

#include "Engine/Core/UUID.h"

//TODO: move implementation into .cpp
namespace Editor
{
	enum class SelectionType
	{
		None = 0,
		Entity,
		Asset
	};

	struct SelectionEntry
	{
		SelectionType type = SelectionType::None;
		Engine::Uuid id = Engine::Uuid{ 0 };

		bool Is(SelectionType otherType) const { return type == otherType; }

		bool operator==(const SelectionEntry& other) const { return type == other.type && id == other.id; }
	};

  class SelectionManager
  {
  public:
    using SelectionCallback = std::function<void(const std::vector<SelectionEntry>&)>;

    static void Subscribe(SelectionCallback callback)
    {
      m_Subscribers.push_back(callback);
    }

    // 1. SELECTING: Now only replaces selection for its OWN type
    static void Select(SelectionType type, Engine::Uuid id, bool additive = false)
    {
      // Remove previous selections of THIS type only, leave others intact
      if(additive)
      {
        // If already selected, do nothing (prevents duplicates)
        for (const auto& entry : m_Selections)
        {
          if (entry.type == type && entry.id == id)
            return;
        }
      }
			else
      {
        ClearInternal(type);
      }

      m_Selections.push_back({ type, id });
      NotifySubscribers();
    }

    // 2. CLEARING BY TYPE: Clear entities without touching materials
    static void Clear(SelectionType type)
    {
      if (ClearInternal(type))
      {
        NotifySubscribers();
      }
    }

    static void ClearAll()
    {
      if (m_Selections.empty()) return;
      m_Selections.clear();
      NotifySubscribers();
    }

    static bool IsSelected(SelectionType type, Engine::Uuid id)
    {
      for (const auto& entry : m_Selections)
      {
        if (entry.type == type && entry.id == id)
          return true;
      }
      return false;
    }

    // 3. GETTING DATA: Fetch the most recently selected item of a specific type
    static Engine::Uuid GetPrimary(SelectionType type)
    {
      // Search backwards to get the most recently clicked item
      for (auto it = m_Selections.rbegin(); it != m_Selections.rend(); ++it)
      {
        if (it->type == type)
          return it->id;
      }
      return 0; // Or whatever your default/invalid UUID is
    }

    // Optional: Get all selected items of a type (for Multi-select)
    static std::vector<Engine::Uuid> GetAll(SelectionType type)
    {
      std::vector<Engine::Uuid> result;
      for (const auto& entry : m_Selections)
      {
        if (entry.type == type)
          result.push_back(entry.id);
      }
      return result;
    }

  private:
    // Helper to remove items without triggering an event (prevents double-firing)
    static bool ClearInternal(SelectionType type)
    {
      auto it = std::remove_if(m_Selections.begin(), m_Selections.end(),
        [type](const SelectionEntry& entry) { return entry.type == type; });

      if (it != m_Selections.end())
      {
        m_Selections.erase(it, m_Selections.end());
        return true; // We actually removed something
      }
      return false;
    }

    static void NotifySubscribers()
    {
      for (const auto& callback : m_Subscribers)
      {
        callback(m_Selections);
      }
    }

    inline static std::vector<SelectionEntry> m_Selections;
    inline static std::vector<SelectionCallback> m_Subscribers;
  };
}