#pragma once

#include "Engine/Graphics/ShaderSpecification.h"

namespace Engine 
{
	class ShaderParser
	{
	public:
		static ShaderSpecification GetSpecification(const std::string& source);
	};
}