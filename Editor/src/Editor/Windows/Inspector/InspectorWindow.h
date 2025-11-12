#pragma once

#include "Editor/Windows/IEditorWindow.h"
#include "EntityInspectorPanel.h"
#include "MaterialInspectorPanel.h"

namespace Editor
{
	class InspectorWindow : public IEditorWindow
	{
	public:
		void OnUpdate(Engine::Timestep ts) override;

	private:
		EntityInspectorPanel m_EntityInspector;
		MaterialInspectorPanel m_MaterialInspector;
	};
}