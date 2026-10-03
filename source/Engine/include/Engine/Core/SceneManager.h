#pragma once

#include "Engine/Core/Scene.h"

namespace engine
{
	struct SceneManager
	{
		static void SetActiveScene(Scene scene);
		static Scene GetActiveScene();
		
	private:
		inline static Scene s_ActiveScene;
	};
}