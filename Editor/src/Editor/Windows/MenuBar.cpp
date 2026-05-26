#include "MenuBar.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Core/EditorWindowManager.h"
#include "Editor/Core/PopupWindowManager.h"
#include "Editor/Utility/File.h"
//#include "Engine/Scene/SceneManager.h"
#include "Editor/Command/SceneCommands.h"
#include "Engine/Core/Project.h"
//#include "Engine/Core/Log.h"
//#include "Engine/Asset/AssetManager.h"
//#include "Engine/Asset/AssetRegistry.h"
//#include "Engine/Graphics/Material.h"
//#include "Engine/Asset/Serializer/MaterialSerializer.h"
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

	static const std::vector<MenuCategory> MainMenuBar
	{
		{ "File", {
			{"Save scene", "ctrl+s", [] {
				Editor::SaveScene();

				//auto records = Engine::AssetManager::GetAssetRegistry().GetRecords(Engine::AssetType::Material);
				//for (auto record : records)
				//{
				//	const Engine::Material& material = Engine::AssetManager::GetAsset<Engine::Material>(record->id);
				//	Engine::MaterialSerializer::Serialize(material, record->path);
				//}
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