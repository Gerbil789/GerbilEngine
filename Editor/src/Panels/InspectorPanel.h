#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Core/Core.h"
#include "Engine/Scene/Entity.h"

namespace Engine
{
	class InspectorPanel
	{
	public:
		InspectorPanel() = default;
		InspectorPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();
	private:
		Ref<Scene> m_Context;

		void DrawComponents(Entity entity);
		void DrawAddComponentButton(Entity entity);
	};
}