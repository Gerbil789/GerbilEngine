#include "Editor/Windows/AssetRegistryWindow.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Core/Project.h"
#include "Editor/Utility/File.h"
#include <imgui.h>
#include <string>
#include <vector>
#include <algorithm>

namespace Editor
{
	void AssetRegistryWindow::Draw()
	{
		const auto& allRecords = Engine::AssetManager::GetAssetRegistry().GetAllRecords();

		ImGui::Begin("Asset Registry");

		static char searchBuffer[256] = "";
		ImGui::InputText("Search ID/Name", searchBuffer, sizeof(searchBuffer));


		const char* typeOptions[] = { "All", "Texture", "Mesh", "Shader", "Material", "Audio", "Scene", "Script" };
		static int selectedTypeIndex = 0;
		ImGui::Combo("Filter Type", &selectedTypeIndex, typeOptions, IM_ARRAYSIZE(typeOptions));

		ImGui::Separator();

		static std::vector<const Engine::AssetRecord*> filteredRecords;
		filteredRecords.clear();
		filteredRecords.reserve(allRecords.size());

		std::string searchStr = searchBuffer;
		std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), ::tolower);

		for (const auto& [id, record] : allRecords)
		{
			if (selectedTypeIndex != 0) 
			{
				Engine::AssetType targetType = static_cast<Engine::AssetType>(selectedTypeIndex);
				if (record.type != targetType) continue;
			}

			if (!searchStr.empty())
			{
				std::string idStr = std::to_string(static_cast<uint64_t>(id));

				std::string nameStr = record.path.filename().string();
				std::transform(nameStr.begin(), nameStr.end(), nameStr.begin(), ::tolower);

				if (idStr.find(searchStr) == std::string::npos && nameStr.find(searchStr) == std::string::npos)
				{
					continue;
				}
			}

			filteredRecords.push_back(&record);
		}

		ImGuiListClipper clipper;
		clipper.Begin(static_cast<int>(filteredRecords.size()));

		while (clipper.Step())
		{
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			{
				const auto* recordPtr = filteredRecords[i];

				ImGui::PushID(static_cast<int>(static_cast<uint64_t>(recordPtr->id)));

				ImGui::Text("Name: %s", recordPtr->path.filename().string().c_str());
				ImGui::Text("ID: %lu", static_cast<uint64_t>(recordPtr->id));
				ImGui::Text("Type: %s", Engine::AssetTypeToString(recordPtr->type).data());

				if (ImGui::Button("Open"))
				{
					Editor::FileDialog::OpenFileExplorer(recordPtr->path);
				}

				ImGui::Spacing();
				ImGui::PopID();
			}
		}

		clipper.End();

		ImGui::Separator();

		if (ImGui::Button("Open registry"))
		{
			Editor::FileDialog::OpenFileExplorer(Engine::Project::GetActive().GetProjectDirectory() / "assetRegistry.json");
		}


		ImGui::End();
	}
}