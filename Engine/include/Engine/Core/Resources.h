#pragma once

#include "Engine/Core/UUID.h"

namespace RESOURCES
{
	namespace MESH
	{
		static constexpr Engine::Uuid EMPTY = Engine::Uuid{ 1 };
		static constexpr Engine::Uuid CUBE = Engine::Uuid{ 2 };
		static constexpr Engine::Uuid SPHERE = Engine::Uuid{ 3 };
	}

	namespace TEXTURE
	{
		static constexpr Engine::Uuid WHITE = Engine::Uuid{ 4 };
		static constexpr Engine::Uuid BLACK = Engine::Uuid{ 5 };
		static constexpr Engine::Uuid NORMAL = Engine::Uuid{ 6 };
		static constexpr Engine::Uuid HDR = Engine::Uuid{ 7 };
	}

	namespace SHADER
	{
		static constexpr Engine::Uuid DEFAULT = Engine::Uuid{ 8 };
		static constexpr Engine::Uuid FLAT = Engine::Uuid{ 9 };
	}

	namespace MATERIAL
	{
		static constexpr Engine::Uuid WHITE = Engine::Uuid{ 10 };
		static constexpr Engine::Uuid PINK = Engine::Uuid{ 11 };
	}

	namespace SCENE
	{
		static constexpr Engine::Uuid DEFAULT = Engine::Uuid{ 12 };
	}
}