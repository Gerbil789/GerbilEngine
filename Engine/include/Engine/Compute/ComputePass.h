#pragma once

#include "Engine/Core/API.h"
#include <webgpu/webgpu.hpp>

namespace Engine
{
	class ENGINE_API ComputePass
	{
	public:
		ComputePass();
		void Execute();
	};
}