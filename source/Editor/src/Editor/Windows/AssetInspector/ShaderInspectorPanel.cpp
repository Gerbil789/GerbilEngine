#include "ShaderInspectorPanel.h"
#include "Engine/Asset/AssetRegistry.h"
#include <imgui.h>
#include <format>

namespace editor
{
	void ShaderInspectorPanel::Draw(engine::ShaderHandle handle)
	{
		const std::string& name = engine::AssetRegistry::GetAssetPath(handle).stem().string();
		ImGui::TextUnformatted(std::format("Shader: {}", name).c_str());
	}
}