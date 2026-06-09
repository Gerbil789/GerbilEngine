#pragma once

namespace Engine
{
	struct Event
	{
		bool Handled = false; //TODO: do i need this?
		virtual ~Event() = default; //TODO: do i need this?
	};
}