#pragma once

#include "Engine/Scene/Material.h"

namespace Engine 
{
	class MaterialPanel
	{
	public:
		MaterialPanel();
		~MaterialPanel();

		void OnImGuiRender();

		void SetMaterial(Ref<Material> material) { m_Material = material; }
	private:
		Ref<Material> m_Material;
	};
}