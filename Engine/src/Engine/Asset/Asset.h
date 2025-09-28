#pragma once

#include "Engine/Core/UUID.h"

namespace Engine
{
	class Asset
	{
	public:
		virtual ~Asset() = default;
		UUID id = UUID(0);
	};
}