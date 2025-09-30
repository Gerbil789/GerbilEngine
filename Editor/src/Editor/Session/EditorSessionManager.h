#pragma once

#include "Editor/Session/SceneEditorSession.h"
#include "Editor/Session/MaterialEditorSession.h"
#include "Engine/Event/Event.h"
#include "Engine/Event/KeyEvent.h"

namespace Editor
{
  class EditorSessionManager
  {
  public:
    static EditorSessionManager& Get()
    {
      static EditorSessionManager instance;
      return instance;
    }

    void OnEvent(Engine::Event& e);
		void OnKeyPressed(Engine::KeyPressedEvent& e);

    SceneEditorSession* GetSceneSession();
    //MaterialEditorSession* GetMaterialSession(UUID materialID);

  private:
    EditorSessionManager() = default;
		SceneEditorSession* m_SceneSession = nullptr;
    //std::unordered_map<UUID, std::unique_ptr<MaterialEditorSession>> m_MaterialSessions;
  };
}