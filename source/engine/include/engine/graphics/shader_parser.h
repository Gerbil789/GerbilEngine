#pragma once

#include "engine/graphics/shader_specification.h"
#include <string_view>

namespace engine
{
	struct ShaderParser
	{
		static ShaderSpecification Parse(std::string_view source);
	};
}