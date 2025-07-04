#include "MenuBar.h"
#include "imgui.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/Application.h"

namespace Editor
{
	using namespace Engine;

	MenuBar::MenuBar()
	{
		//load textures
		//m_Icon_Play = AssetManager::GetAsset<Texture2D>("resources/icons/play.png");
		//m_Icon_Pause = AssetManager::GetAsset<Texture2D>("resources/icons/pause.png");
		//m_Icon_Next = AssetManager::GetAsset<Texture2D>("resources/icons/skip_next.png");
	}
	void MenuBar::OnUpdate()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "ctrl + N"))
				{
					SceneManager::CreateScene("New Scene");
				}

				if (ImGui::MenuItem("Open", "ctrl + O"))
				{
					SceneManager::LoadScene();
				}

				if (ImGui::MenuItem("Save", "ctrl + S"))
				{
					SceneManager::SaveScene();
					auto assets = AssetManager::GetLoadedAssets();
					for (auto asset : assets)
					{
						if (asset->IsModified())
						{
							//TODO: save asset
							LOG_INFO("Asset '{0}' is modified", asset->GetFilePath());
						}

					}


				}

				if (ImGui::MenuItem("Save as", "ctrl + shift + S"))
				{
					SceneManager::SaveSceneAs();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Settings")) { /*m_SettingsPanel.SetVisible(true);*/ }

				ImGui::Separator();

				if (ImGui::MenuItem("Exit")) { Application::Get().Close(); }

				ImGui::EndMenu();
			}


			//if (ImGui::BeginMenu("Windows"))
			//{
			//	if (ImGui::MenuItem("Reset layout"))
			//	{
			//		ImGuiLayer::ResetLayout();
			//	}
			//	ImGui::EndMenu();
			//}

			// ---- BUTTONS ----
			//ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 30);

			//if (m_CurrentScene->IsPlaying())
			//{
			//	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
			//}
			//else
			//{
			//	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			//}

			//if (ImGui::ImageButton((ImTextureID)m_Icon_Play->GetRendererID(), { 20, 20 }, { 0, 1 }, { 1, 0 }))
			//{
			//	if (m_CurrentScene->IsPlaying())
			//	{
			//		m_CurrentScene->OnStop();
			//	}
			//	else
			//	{
			//		m_CurrentScene->OnPlay();
			//	}

			//}

			//ImGui::PopStyleColor(1);

			//if (ImGui::ImageButton((ImTextureID)m_Icon_Pause->GetRendererID(), { 20, 20 }, { 0, 1 }, { 1, 0 }))
			//{

			//}

			//if (ImGui::ImageButton((ImTextureID)m_Icon_Next->GetRendererID(), { 20, 20 }, { 0, 1 }, { 1, 0 }))
			//{

			//}

			ImGui::EndMenuBar();
		}
	}
}