#include "ShaderInspectorPanel.h"
#include "Engine/Graphics/Shader.h"
#include <imgui.h>
#include <format>

namespace editor
{
	void ShaderInspectorPanel::Draw(engine::Shader shader)
	{
		ImGui::TextUnformatted(std::format("Shader: {}", static_cast<uint64_t>(shader)).c_str());

		//ImGui::Separator();

		//const Engine::ShaderSpecification& spec = shader->GetSpecification();
		//ImGui::TextUnformatted("Vertex Attributes:");
		//for (const Engine::VertexAttribute& attr : spec.vertexAttributes)
		//{
		//	ImGui::TextUnformatted((" - " + attr.label + " : " + Engine::VertexFormatToString(attr.format)).c_str());
		//}

		//ImGui::Separator();

		//ImGui::TextUnformatted("Bindings:");
		//for (const auto& binding : spec.bindings)
		//{
		//	ImGui::TextUnformatted((" - " + binding.name + " (Group " + std::to_string(binding.group) + ", Binding " + std::to_string(binding.binding) + ")").c_str());
		//}
	}
}