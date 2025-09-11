#pragma once

namespace Engine
{
	class Asset
	{
	public:
		virtual ~Asset() = default;
		UUID id = UUID(0);
	};
}