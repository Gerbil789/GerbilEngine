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
		static constexpr Engine::Uuid EDITOR_ICONS = Engine::Uuid{ 13 };
		static constexpr Engine::Uuid DEFAULT_FONT_ATLAS = Engine::Uuid{ 15 };
	}

	namespace SHADER
	{
		static constexpr Engine::Uuid DEFAULT = Engine::Uuid{ 8 };
		static constexpr Engine::Uuid FLAT = Engine::Uuid{ 9 };
		static constexpr Engine::Uuid UI = Engine::Uuid{ 10 };
	}

	namespace MATERIAL
	{
		static constexpr Engine::Uuid WHITE = Engine::Uuid{ 12 };
		static constexpr Engine::Uuid PINK = Engine::Uuid{ 13 };
	}

	namespace SCENE
	{
		static constexpr Engine::Uuid DEFAULT = Engine::Uuid{ 14 };
	}

	namespace FONT
	{
		static constexpr Engine::Uuid DEFAULT = Engine::Uuid{ 15 };
	}
}