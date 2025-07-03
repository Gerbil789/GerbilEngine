#pragma once

#include "EditorWindow.h"
#include "Engine/Renderer/Material.h"
#include "Editor/Core/Core.h"

namespace Editor
{
	class MaterialWindow : public EditorWindow
	{
	public:
		MaterialWindow(EditorWindowManager* context);
		~MaterialWindow();
		
		void OnUpdate(Engine::Timestep ts) override;

		void SetMaterial(Ref<Engine::Material> material) { m_Material = material; }
	private:
		Ref<Engine::Material> m_Material;

		std::vector<std::filesystem::path> m_ShaderPaths;

		bool IsColorProperty(const std::string& name);
	};
}