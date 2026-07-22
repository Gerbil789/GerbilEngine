#pragma once

namespace Editor
{
	class IPopup
	{
	public:
		virtual ~IPopup() = default;

		virtual const char* GetID() const = 0;
		virtual void Open() = 0;
		virtual void Draw() = 0;
	};
}