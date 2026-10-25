#include "ShaderInspectorPanel.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Asset/AssetRegistry.h"
#include <imgui.h>
#include <format>

namespace editor
{
	void ShaderInspectorPanel::Draw(engine::Shader shader)
	{
		const std::string& name = engine::AssetRegistry::GetAssetPath(shader).stem().string();
		ImGui::TextUnformatted(std::format("Shader: {}", name).c_str());
	}
}