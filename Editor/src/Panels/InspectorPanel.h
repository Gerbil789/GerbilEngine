#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Core/Core.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/SceneManager.h"

namespace Engine
{
	class InspectorPanel : public ISceneObserver
	{
	public:
		InspectorPanel();
		~InspectorPanel();

		void OnSceneChanged() override;
		void OnImGuiRender();

	private:
		Ref<Scene> m_Scene;

		void DrawComponents(Entity entity);
		void DrawAddComponentButton(Entity entity);
	};
}