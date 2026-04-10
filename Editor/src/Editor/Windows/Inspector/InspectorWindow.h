#pragma once

#include "Editor/Windows/IEditorWindow.h"
#include "EntityInspectorPanel.h"
#include "ShaderInspectorPanel.h"
#include "AudioInspectorPanel.h"

namespace Editor
{
	class InspectorWindow : public IEditorWindow
	{
	public:
		void Initialize() override;
		void Draw() override;

	private:
		void DrawAssetPanel(Engine::Uuid assetID);

	private:
		EntityInspectorPanel m_EntityInspector;
		ShaderInspectorPanel m_ShaderInspector;
		AudioInspectorPanel m_AudioInspector;
	};
}