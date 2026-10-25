#include "MaterialEditorWindow.h"
#include "Editor/Windows/Utility/Property.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Event/EventBus.h"
#include "Editor/Core/EditorEvent.h"
#include <imgui.h>

namespace editor
{
	namespace
	{
		engine::MaterialInstance m_MaterialInstance = engine::MaterialInstance::Null;
	}

	void MaterialEditorWindow::Draw()
	{
		ImGui::Begin("Material Instance");

		if (m_MaterialInstance == engine::MaterialInstance::Null)
		{
			ImGui::Text("No material selected");
			ImGui::End();
			return;
		}

		const std::string& materialInstanceName = engine::AssetRegistry::GetAssetPath(m_MaterialInstance).stem().string();
		ImGui::Text("Material instance: %s", materialInstanceName.c_str());

		engine::MaterialInstanceData& instanceData = m_MaterialInstance.Get();
		engine::Material material = instanceData.material;
		engine::Shader shader = material.Get().shader;

		const std::string& materialName = engine::AssetRegistry::GetAssetPath(material).stem().string();
		const std::string& shaderName = engine::AssetRegistry::GetAssetPath(shader).stem().string();

		ImGui::Text("Material: %s", materialName.c_str());
		ImGui::Text("Shader: %s", shaderName.c_str());

		if (ImGui::BeginTable("MaterialProperties", 2, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

			auto bindings = shader.GetMaterialBindings();

			auto isColorParameter = [](const std::string& name) {
				std::string lowerName = name;
				std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
				return lowerName.find("color") != std::string::npos ||
					lowerName.find("albedo") != std::string::npos ||
					lowerName.find("tint") != std::string::npos ||
					lowerName.find("emission") != std::string::npos ||
					lowerName.find("specular") != std::string::npos;
				};

			for (auto& binding : bindings)
			{
				if (std::holds_alternative<engine::BufferBinding>(binding.data))
				{
					const auto& bufferBinding = std::get<engine::BufferBinding>(binding.data);

					for (auto& param : bufferBinding.parameters)
					{
						if (param.name.empty() || param.name[0] == '_') continue;

						char* bufferPtr = reinterpret_cast<char*>(instanceData.uniformData.data());

						switch (param.type)
						{
						case engine::ShaderParamType::Float: {
							float value;
							std::memcpy(&value, bufferPtr + param.offset, sizeof(float));

							if (PropertyField<float>(param.name.c_str(), value).changed)
							{
								std::memcpy(bufferPtr + param.offset, &value, sizeof(float));
							}
							break;
						}
						case engine::ShaderParamType::Int: {
							int32_t value;
							std::memcpy(&value, bufferPtr + param.offset, sizeof(int32_t));

							if (PropertyField<int32_t>(param.name.c_str(), value).changed)
							{
								std::memcpy(bufferPtr + param.offset, &value, sizeof(int32_t));
							}
							break;
						}
						case engine::ShaderParamType::Uint: {
							uint32_t value;
							std::memcpy(&value, bufferPtr + param.offset, sizeof(uint32_t));

							if (PropertyField<uint32_t>(param.name.c_str(), value).changed)
							{
								std::memcpy(bufferPtr + param.offset, &value, sizeof(uint32_t));
							}
							break;
						}
						case engine::ShaderParamType::Bool: {
							int32_t rawVal;
							std::memcpy(&rawVal, bufferPtr + param.offset, sizeof(int32_t));
							bool value = static_cast<bool>(rawVal);

							if (PropertyField<bool>(param.name.c_str(), value).changed)
							{
								int32_t newRawVal = value ? 1 : 0;
								std::memcpy(bufferPtr + param.offset, &newRawVal, sizeof(int32_t));
							}
							break;
						}
						case engine::ShaderParamType::Vec2: {
							glm::vec2 value;
							std::memcpy(&value, bufferPtr + param.offset, sizeof(glm::vec2));

							if (PropertyField<glm::vec2>(param.name.c_str(), value).changed)
							{
								std::memcpy(bufferPtr + param.offset, &value, sizeof(glm::vec2));
							}
							break;
						}
						case engine::ShaderParamType::IVec2: {
							glm::ivec2 value;
							std::memcpy(&value, bufferPtr + param.offset, sizeof(glm::ivec2));

							if (PropertyField<glm::ivec2>(param.name.c_str(), value).changed)
							{
								std::memcpy(bufferPtr + param.offset, &value, sizeof(glm::ivec2));
							}
							break;
						}
						case engine::ShaderParamType::Vec3: {
							glm::vec3 value;
							std::memcpy(&value, bufferPtr + param.offset, sizeof(glm::vec3));

							DisplayMode mode = isColorParameter(param.name) ? DisplayMode::Color : DisplayMode::Default;

							if (PropertyField<glm::vec3>(param.name.c_str(), value, { .mode = mode }).changed)
							{
								std::memcpy(bufferPtr + param.offset, &value, sizeof(glm::vec3));
							}
							break;
						}
						case engine::ShaderParamType::Vec4: {
							glm::vec4 value;
							std::memcpy(&value, bufferPtr + param.offset, sizeof(glm::vec4));

							DisplayMode mode = isColorParameter(param.name) ? DisplayMode::Color : DisplayMode::Default;

							if (PropertyField<glm::vec4>(param.name.c_str(), value, { .mode = mode }).changed)
							{
								std::memcpy(bufferPtr + param.offset, &value, sizeof(glm::vec4));
							}
							break;
						}
						case engine::ShaderParamType::Mat3: {
							glm::mat3 value;
							std::memcpy(&value, bufferPtr + param.offset, sizeof(glm::mat3));

							if (PropertyField<glm::mat3>(param.name.c_str(), value).changed)
							{
								std::memcpy(bufferPtr + param.offset, &value, sizeof(glm::mat3));
							}
							break;
						}
						case engine::ShaderParamType::Mat4: {
							glm::mat4 value;
							std::memcpy(&value, bufferPtr + param.offset, sizeof(glm::mat4));

							if (PropertyField<glm::mat4>(param.name.c_str(), value).changed)
							{
								std::memcpy(bufferPtr + param.offset, &value, sizeof(glm::mat4));
							}
							break;
						}
						default:
							break;
						}
					}
				}
				else if (std::holds_alternative<engine::TextureBinding>(binding.data))
				{
					engine::Texture2D texture = instanceData.textures.at(binding.binding);
					if (AssetField(binding.name, texture).changed)
					{
						instanceData.textures[binding.binding] = texture;
						instanceData.CreateBindGroup();
					}
				}
			}

			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				ImGui::TextUnformatted("Texture Filter");
				ImGui::TableSetColumnIndex(1);
				const char* filterOptions[] = { "Nearest", "Bilinear", "Trilinear" };
				int currentFilter = static_cast<int>(instanceData.textureFilter);
				if (ImGui::Combo("##TextureFilter", &currentFilter, filterOptions, IM_ARRAYSIZE(filterOptions)))
				{
					instanceData.textureFilter = static_cast<engine::TextureFilter>(currentFilter);
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted("Texture Wrap");
				ImGui::TableSetColumnIndex(1);
				const char* wrapOptions[] = { "Repeat", "Clamp", "Mirror" };
				int currentWrap = static_cast<int>(instanceData.textureWrap);
				if (ImGui::Combo("##TextureWrap", &currentWrap, wrapOptions, IM_ARRAYSIZE(wrapOptions)))
				{
					instanceData.textureWrap = static_cast<engine::TextureWrap>(currentWrap);
				}
			}

			ImGui::EndTable();
		}

		ImGui::End();
	}

	void MaterialEditorWindow::Initialize()
	{
		engine::EventBus::Subscribe<SelectAssetEvent>([](const SelectAssetEvent& e)
			{
				engine::AssetType type = engine::AssetRegistry::GetAssetType(e.id);
				if (type != engine::AssetType::MaterialInstance) return false;
				m_MaterialInstance = engine::MaterialInstance{ e.id };
				return false;
			});
	}
}