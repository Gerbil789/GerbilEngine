#include "menu_bar.h"
#include "editor/utility/file.h"
#include "engine/core/context.h"
#include "engine/serializer/serializer.h"
#include "engine/core/context.h"
#include "engine/asset/asset_manager.h"
#include <imgui.h>

namespace editor
{
	struct MenuEntry
	{
		const char* label;
		const char* shortcut;
		std::function<void()> action;
	};

	struct MenuCategory
	{
		const char* name;
		std::vector<MenuEntry> entries;
	};

	inline void SaveScene()
	{
		engine::SceneHandle handle = engine::context::activeSceneHandle;
		auto& path = engine::assetRegistry::GetassetPath(handle);

		engine::serializer::SerializeScene(handle, engine::context::assetsDirectory / path);
	}

	static const std::vector<MenuCategory> MainMenuBar
	{
		{ "File", {
			{"Save scene", "ctrl+s", [] {
				SaveScene();
				engine::assetManager::SaveDirtyassets();
			}},
			//{"Open scene", "", [] { editor::OpenScene(); },
		}},

		// { "Edit", {
		// 	{"Undo", "Ctrl+Z", EditorCommandManager::Undo},
		// 	{"Redo", "Ctrl+Shift+Z", EditorCommandManager::Redo},
		// }},

		// { "Project", {
		// 	{"New", "", [] { PopupManager::Open("New Project"); }},
		// 	{"Open", "", [] { engine::Project::Load(editor::FileDialog::SelectDirectory()); }}, //TODO: select project.json file instead of directory?
		// 	{"settings", "", [] { /*TODO*/ }},
		// 	{"Save layout", "", [] { editor::editorWindowManager::SaveLayout(); }},
		// }},

		{ "Debug", {
			//{"RenderDoc", "", [] { /*TODO*/ }}
		}}
	};

	void MenuBar::Draw()
	{
		if (ImGui::BeginMenuBar())
		{
			for (const auto& category : MainMenuBar)
			{
				if (ImGui::BeginMenu(category.name))
				{
					for (const auto& e : category.entries)
					{
						if (ImGui::MenuItem(e.label, e.shortcut))
						{
							if (e.action) e.action();
						}
					}
					ImGui::EndMenu();
				}
			}
			ImGui::EndMenuBar();
		}
	}
}