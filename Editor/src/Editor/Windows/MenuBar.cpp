#include "MenuBar.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/Application.h"
#include "Editor/Command/CommandManager.h"
#include "Editor/Core/EditorWindowManager.h"
#include <imgui.h>

namespace Editor
{
	void MenuBar::OnUpdate()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "ctrl + N"))
				{
					//SceneManager::CreateScene("NewScene");
				}

				if (ImGui::MenuItem("Open", "ctrl + O"))
				{
					//SceneManager::LoadScene();
				}

				if (ImGui::MenuItem("Save", "ctrl + S"))
				{
					Engine::SceneManager::SaveScene();
					//auto assets = AssetManager::GetLoadedAssets();
					//for (auto asset : assets)
					//{
					//	if (asset->IsModified())
					//	{
					//		//TODO: save asset
					//		LOG_INFO("Asset '{0}' is modified", asset->GetFilePath());
					//	}

					//}


				}

				if (ImGui::MenuItem("Save as", "ctrl + shift + S"))
				{
					Engine::SceneManager::SaveSceneAs();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Settings")) { /*m_SettingsPanel.SetVisible(true);*/ }

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Actions"))
			{
				if (ImGui::MenuItem("Undo"))
				{
					CommandManager::Undo();
				}

				if (ImGui::MenuItem("Redo"))
				{
					CommandManager::Redo();
				}

				ImGui::EndMenu();
			}
			

			ImGui::EndMenuBar();
		}
	}
}