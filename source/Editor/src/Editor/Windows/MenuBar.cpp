#include "MenuBar.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Core/EditorWindowManager.h"
#include "Editor/Core/PopupWindowManager.h"
#include "Editor/Utility/File.h"
#include "Engine/Core/Project.h"
#include "Engine/Asset/Serializer/MaterialSerializer.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Asset/AssetManager.h"
#include <imgui.h>

namespace Editor
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
		Engine::Uuid sceneId = Engine::SceneManager::GetActiveScene();
		auto& path = Engine::AssetManager::GetAssetPath(sceneId);

		//if (path.empty())
		//{
		//	path = Editor::FileDialog::SelectPath({ {"Scene Files", "*.scene"} }, "scene"); //prompt user to select path
		//}

		Engine::SceneSerializer::Serialize(sceneId, Engine::Project::GetActive().GetAssetsDirectory() / path);
	}

	static const std::vector<MenuCategory> MainMenuBar
	{
		{ "File", {
			{"Save scene", "ctrl+s", [] {
				SaveScene();
				Engine::AssetManager::SaveDirtyAssets();
			}},
			//{"Open scene", "", [] { Editor::OpenScene(); },
		}},

		{ "Edit", {
			{"Undo", "Ctrl+Z", EditorCommandManager::Undo},
			{"Redo", "Ctrl+Shift+Z", EditorCommandManager::Redo},
		}},

		{ "Project", {
			{"New", "", [] { PopupManager::Open("New Project"); }},
			{"Open", "", [] { Engine::Project::Load(Editor::FileDialog::SelectDirectory()); }}, //TODO: select project.json file instead of directory?
			{"Settings", "", [] { /*TODO*/ }},
			{"Save layout", "", [] { Editor::EditorWindowManager::SaveLayout(); }},
		}},

		{ "Debug", {
			{"RenderDoc", "", [] { /*TODO*/ }}
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