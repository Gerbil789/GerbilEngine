#pragma once

namespace Engine 
{
	class EditorWindowBase
	{
	public:
		EditorWindowBase() = default;
		virtual ~EditorWindowBase() = default;

		virtual void OnImGuiRender() = 0;

		bool IsVisible() const { return m_IsVisible; }
		void SetVisible(bool visible) { m_IsVisible = visible; }

	protected:
		bool m_IsVisible = false;
	};
}
