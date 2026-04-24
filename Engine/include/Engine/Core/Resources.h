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

	//static constexpr Uuid WHITE_TEXTURE = Uuid{ 11 };
	//static constexpr Uuid BLACK_TEXTURE = Uuid{ 12 };
	//static constexpr Uuid NORMAL_TEXTURE = Uuid{ 13 };
}