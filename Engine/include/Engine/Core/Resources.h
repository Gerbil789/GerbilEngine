#pragma once

#include "Engine/Core/UUID.h"

namespace Engine::RESOURCES
{
	static constexpr Uuid INVALID_UUID = Uuid{ 0 };

	namespace MESH
	{
		static constexpr Uuid CUBE = Uuid{ 1 };
		static constexpr Uuid SPHERE = Uuid{ 2 };
	}

	namespace TEXTURE
	{
		static constexpr Uuid WHITE = Uuid{ 3 };
		static constexpr Uuid BLACK = Uuid{ 4 };
		static constexpr Uuid NORMAL = Uuid{ 5 };
		static constexpr Uuid HDR = Uuid{ 6 };
	}
}