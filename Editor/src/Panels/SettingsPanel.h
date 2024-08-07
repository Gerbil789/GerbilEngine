#pragma once

namespace Engine
{
	class SettingsPanel
	{
	public:
		SettingsPanel() = default;
		~SettingsPanel() = default;

		void OnImGuiRender();

		bool IsVisible() const { return m_IsVisible; }
		void SetVisible(bool visible) { m_IsVisible = visible; }

	private:
		bool m_IsVisible = false;
	};
}