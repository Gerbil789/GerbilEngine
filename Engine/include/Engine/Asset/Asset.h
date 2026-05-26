#pragma once

#include "Engine/Core/UUID.h"
#include <string>

namespace Engine
{
	class ENGINE_API Asset
	{
	public:
		Uuid id = Uuid{ 0 };

#ifdef GERBIL_EDITOR
		std::string editor_name = "Unnamed Asset";
#endif

	};
}