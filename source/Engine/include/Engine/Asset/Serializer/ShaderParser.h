#pragma once

#include "Engine/Graphics/ShaderSpecification.h"
#include <string_view>

namespace Engine
{
	class ShaderParser
	{
	public:
		static ShaderSpecification Parse(std::string_view source);
	};
}