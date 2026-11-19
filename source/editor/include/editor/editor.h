#pragma once

#include "engine/core/application.h"

namespace editor
{
	struct Editor
	{
		Editor(const engine::Application& app);
		~Editor();
		void Update();

	private:
    engine::Application m_app;
	};
}