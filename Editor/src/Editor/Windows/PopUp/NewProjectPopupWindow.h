#pragma once

#include "Editor/Windows/IPopup.h"

namespace Editor
{
	class NewProjectPopupWindow : public IPopup
	{
	public:
		const char* GetID() const override { return "New Project"; }

		void Open() override
		{
			m_OpenRequested = true;
		}

		void Draw() override;
		
	private:
		bool m_OpenRequested = false;
	};
}