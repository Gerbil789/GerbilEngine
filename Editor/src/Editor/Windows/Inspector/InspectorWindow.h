#pragma once

#include "Editor/Windows/IEditorWindow.h"
#include "EntityInspectorPanel.h"
#include "MaterialInspectorPanel.h"
#include "ShaderInspectorPanel.h"

namespace Editor
{
	class InspectorWindow : public IEditorWindow
	{
	public:
		void OnUpdate(Engine::Timestep ts) override;

	private:
		void DrawAssetPanel(Engine::UUID assetID);

	private:
		EntityInspectorPanel m_EntityInspector;
		MaterialInspectorPanel m_MaterialInspector;
		ShaderInspectorPanel m_ShaderInspector;
	};
}