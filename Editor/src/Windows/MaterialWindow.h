#pragma once

#include "EditorWindowBase.h"
#include "Engine/Scene/Material.h"

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
	};
}