#pragma once

#include "EditorWindowBase.h"
#include "Engine/Renderer/Material.h"

namespace Engine 
{
	class MaterialWindow : public EditorWindowBase
	{
	public:
		MaterialWindow();
		~MaterialWindow();

		void OnImGuiRender() override;

		void SetMaterial(Ref<Material> material) { m_Material = material; }
	private:
		Ref<Material> m_Material;

		std::vector<std::filesystem::path> m_ShaderPaths;

		bool IsColorProperty(const std::string& name);
	};
}