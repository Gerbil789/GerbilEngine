#pragma once

#include "Engine/Events/Event.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Core/Core.h"

namespace Editor::EditorSceneController
{
	void Initialize();

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