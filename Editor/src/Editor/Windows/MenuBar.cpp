#include "MenuBar.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Core/PopupWindowManager.h"
#include "Engine/Utils/File.h"
#include "Editor/Core/Project.h"
#include "Engine/Core/Log.h"
#include <imgui.h>

namespace Editor
{
	struct ScopedMenuBar
	{
		bool open;
		ScopedMenuBar() : open(ImGui::BeginMenuBar()) {}
		~ScopedMenuBar() { if (open) ImGui::EndMenuBar(); }
		explicit operator bool() const { return open; }
	};

	struct ScopedMenu
	{
		bool open;
		explicit ScopedMenu(const char* label) : open(ImGui::BeginMenu(label)) {}
		~ScopedMenu() { if (open) ImGui::EndMenu(); }
		explicit operator bool() const { return open; }
	};

	using MenuAction = void(*)();

	struct MenuEntry
	{
		const char* label;
		const char* shortcut;
		MenuAction action;
	};

	static const MenuEntry FileMenu[]
	{
		{"New",  "Ctrl+N", [] { LOG_WARNING("New File - not implemented");} },
		{"Open", "Ctrl+O", [] { LOG_WARNING("Open File - not implemented");} },
		{"Save", "Ctrl+S", Engine::SceneManager::SaveScene},
	};

	static const MenuEntry EditMenu[]
	{
		{"Undo", "Ctrl+Z", EditorCommandManager::Undo},
		{"Redo", "Ctrl+Shift+Z", EditorCommandManager::Redo},
	};

	static const MenuEntry ProjectMenu[]
	{
		{"New", "", [] { PopupManager::Open("New Project"); }},
		{"Open", "", [] { Editor::Project::Load(Engine::OpenDirectory()); }},
		{"Settings", "", [] { LOG_WARNING("Project Settings - not implemented"); }},
	};

	void MenuBar::Draw()
	{
		if (ScopedMenuBar bar{})
		{

			if (ScopedMenu file{ "File" })
			{
				for (auto& e : FileMenu)
				{
					if (ImGui::MenuItem(e.label, e.shortcut))
					{
						e.action();
					}
				}
			}

			if (ScopedMenu edit{ "Edit" })
			{
				for (auto& e : EditMenu)
				{
					if (ImGui::MenuItem(e.label, e.shortcut))
					{
						e.action();
					}
				}
			}

			if (ScopedMenu project{ "Project" })
			{
				for (auto& e : ProjectMenu)
				{
					if (ImGui::MenuItem(e.label, e.shortcut))
					{
						e.action();
					}
				}
			}
		}
	}
}