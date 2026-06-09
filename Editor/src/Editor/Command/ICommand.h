#pragma once

namespace Editor
{
	class ICommand
	{
	public:
		virtual ~ICommand() = default;
		virtual void Execute() = 0;
		virtual void Undo() = 0; // TODO: make undo optional? some commands dont implement it anyway...
	};
}