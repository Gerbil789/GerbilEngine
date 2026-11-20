#include "menu_bar.h"
#include "engine/core/context.h"
#include "engine/serializer/serializer.h"
#include "engine/asset/asset_registry.h"
#include "engine/core/command_manager.h"
#include <imgui.h>

namespace editor
{
	void MenuBar::Draw()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save scene"))
				{
					engine::CommandManager::Enqueue([]{
						const std::filesystem::path& path = engine::assetRegistry::GetassetPath(engine::context::activeSceneHandle);
						engine::serializer::SerializeScene(engine::context::activeSceneHandle, engine::context::assetsDirectory / path);
					});
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}
}