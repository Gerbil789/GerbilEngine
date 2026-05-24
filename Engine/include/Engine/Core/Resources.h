#pragma once

#include "Engine/Core/UUID.h"

namespace RESOURCES
{
	static constexpr Engine::Uuid INVALID_UUID = Engine::Uuid{ 0 };

	namespace MESH
	{
		static constexpr Engine::Uuid CUBE = Engine::Uuid{ 1 };
		static constexpr Engine::Uuid SPHERE = Engine::Uuid{ 2 };
	}

	namespace TEXTURE
	{
		static constexpr Engine::Uuid WHITE = Engine::Uuid{ 3 };
		static constexpr Engine::Uuid BLACK = Engine::Uuid{ 4 };
		static constexpr Engine::Uuid NORMAL = Engine::Uuid{ 5 };
		static constexpr Engine::Uuid HDR = Engine::Uuid{ 6 };
	}

	namespace SHADER
	{
		static constexpr Engine::Uuid DEFAULT = Engine::Uuid{ 7 };
	}

	namespace MATERIAL
	{
		static constexpr Engine::Uuid WHITE = Engine::Uuid{ 8 };
		static constexpr Engine::Uuid PINK = Engine::Uuid{ 9 };
	}

	namespace SCENE
	{
		static constexpr Engine::Uuid DEFAULT = Engine::Uuid{ 10 };
	}
}