#pragma once

#include "Engine/Core/Application.h"

namespace editor
{
	class Editor
	{
	public:
		Editor(const engine::Application& app);
		~Editor();
		void Update();

	private:
    engine::Application m_App;
	};
}