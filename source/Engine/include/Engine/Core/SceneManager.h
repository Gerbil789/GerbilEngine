#pragma once

#include "Engine/Core/Scene.h"

namespace engine
{
	struct SceneManager
	{
		static void SetActiveScene(SceneHandle scene);
		static SceneHandle GetActiveScene();
		
	private:
		inline static SceneHandle s_ActiveScene;
	};
}