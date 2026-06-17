#include "ShaderInspectorPanel.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include <imgui.h>

namespace Editor
{
	void ShaderInspectorPanel::Draw(Engine::Uuid id)
	{
		ImGui::TextUnformatted(std::format("Shader: {}", id).c_str());

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