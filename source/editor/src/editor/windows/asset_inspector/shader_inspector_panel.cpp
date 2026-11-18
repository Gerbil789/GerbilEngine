#include "shader_inspector_panel.h"
#include "engine/asset/asset_registry.h"
#include <imgui.h>
#include <format>

namespace editor
{
	void ShaderInspectorPanel::Draw(engine::ShaderHandle handle)
	{
		const std::string& name = engine::assetRegistry::GetassetPath(handle).stem().string();
		ImGui::TextUnformatted(std::format("Shader: {}", name).c_str());
	}
}