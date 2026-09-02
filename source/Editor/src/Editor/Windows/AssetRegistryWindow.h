#pragma once

#include "IEditorWindow.h"

namespace editor
{
	class AssetRegistryWindow : public IEditorWindow
	{
	public:
		void Draw() override;
	};
}