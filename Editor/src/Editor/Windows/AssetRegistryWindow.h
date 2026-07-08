#pragma once

#include "IEditorWindow.h"

namespace Editor
{
	class AssetRegistryWindow : public IEditorWindow
	{
	public:
		void Draw() override;
	};
}