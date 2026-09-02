#pragma once

#include "Engine/Core/UUID.h"

namespace engine
{
	class Asset
	{
	public:
		Uuid id;

		Asset(const Asset&) = delete;
		Asset& operator=(const Asset&) = delete;

		Asset(Asset&&) noexcept = default;
		Asset& operator=(Asset&&) noexcept = default;

	protected:
		Asset() = default;
	};
}