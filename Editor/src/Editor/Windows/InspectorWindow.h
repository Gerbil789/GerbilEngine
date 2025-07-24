#pragma once

#include "EditorWindow.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/SceneManager.h"

namespace Editor
{
	class InspectorWindow : public EditorWindow
	{
	public:
		void OnUpdate(Engine::Timestep ts) override;

	private:
		void DrawComponents(Engine::Entity entity);
		void DrawAddComponentButton(Engine::Entity entity);

	};
}