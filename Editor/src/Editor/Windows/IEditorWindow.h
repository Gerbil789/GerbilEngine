#pragma once

namespace Editor
{
	class IEditorWindow
	{
	public:
		virtual ~IEditorWindow() = default;
		virtual void Initialize() {}
		virtual void Draw() = 0;
	};
}
