#include "MenuBar.h"
#include "Engine/Scene/SceneManager.h"
#include "Editor/Command/EditorCommandManager.h"
#include "Editor/Core/PopupWindowManager.h"
#include "Engine/Utility/File.h"
#include "Engine/Core/Project.h"
#include "Engine/Core/Log.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Asset/Serializer/MaterialSerializer.h"
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

	struct MenuEntry
	{
		const char* label;
		const char* shortcut;
		std::function<void()> action;
	};

	static const MenuEntry FileMenu[]
	{
		{"New",  "Ctrl+N", [] { LOG_WARNING("New File - not implemented");} },
		{"Open", "Ctrl+O", [] { LOG_WARNING("Open File - not implemented");} },
		{"Save", "Ctrl+S", [] {
			Engine::SceneManager::SaveScene();

			auto records = Engine::AssetManager::GetAssetRegistry().GetRecords(Engine::AssetType::Material);
			for (auto record : records)
			{
				Engine::Material& material = Engine::AssetManager::GetAsset<Engine::Material>(record->id);
				auto path = Engine::AssetManager::GetAssetRegistry().GetPath(material.id);
				Engine::MaterialSerializer::Serialize(&material, path);
			}
		}},
	};

	static const MenuEntry EditMenu[]
	{
		{"Undo", "Ctrl+Z", EditorCommandManager::Undo},
		{"Redo", "Ctrl+Shift+Z", EditorCommandManager::Redo},
	};

	static const MenuEntry ProjectMenu[]
	{
		{"New", "", [] { PopupManager::Open("New Project"); }},
		{"Open", "", [] { Engine::Project::Load(Engine::OpenDirectory()); }},
		{"Settings", "", [] { LOG_WARNING("Project Settings - not implemented"); }},
	};

	static const MenuEntry DebugMenu[]
	{
		{"RenderDoc", "", [] {} }
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

			if (ScopedMenu debug{ "Debug" })
			{
				for (auto& e : DebugMenu)
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