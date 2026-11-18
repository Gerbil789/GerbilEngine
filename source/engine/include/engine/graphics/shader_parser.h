#pragma once

#include "engine/graphics/shader_specification.h"
#include <string_view>

namespace engine
{
	class ShaderParser
	{
	public:
		static ShaderSpecification Parse(std::string_view source);
	};
}