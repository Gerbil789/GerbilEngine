#pragma once

#include "Engine/Event/Event.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/SceneManager.h"

namespace Editor::EditorSceneController
{
	void OnEvent(Engine::Event& e);

	void DuplicateEntity(Engine::Entity entity);
	//void CopyEntity(Entity entity);
	//void PasteEntity();
	void SelectEntity(Engine::Entity entity);
	void DeselectEntity();
	bool IsEntitySelected(Engine::Entity entity);
	Engine::Entity GetSelectedEntity();

	bool OnKeyPressed(Engine::KeyPressedEvent& e);


}