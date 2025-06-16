#pragma once

namespace Engine
{
	class MenuBar
	{
	public:
		MenuBar();
		~MenuBar() = default;

		void OnImGuiRender();

	private:
		//Ref<Texture2D> m_Icon_Play;
		//Ref<Texture2D> m_Icon_Pause;
		//Ref<Texture2D> m_Icon_Next;
	};
}