#pragma once

#include "Editor/Windows/IEditorWindow.h"
#include "EntityInspectorPanel.h"
#include "MaterialInspectorPanel.h"
#include "ShaderInspectorPanel.h"
#include "AudioInspectorPanel.h"

namespace Editor
{
	class InspectorWindow : public IEditorWindow
	{
	public:
		void Draw() override;

	private:
		void DrawAssetPanel(Engine::Uuid assetID);

	private:
		EntityInspectorPanel m_EntityInspector;
		MaterialInspectorPanel m_MaterialInspector;
		ShaderInspectorPanel m_ShaderInspector;
		AudioInspectorPanel m_AudioInspector;
	};
}