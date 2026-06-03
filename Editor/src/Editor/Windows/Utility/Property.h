#pragma once

#include "Engine/Core/UUID.h"
#include <string>
#include <imgui.h>
#include <glm/glm.hpp>
#include <limits>

namespace Engine { class Texture2D; class AudioClip; class Mesh; class Material; class TextureCube; class Shader; enum class AssetType;}

namespace Editor
{
	namespace
	{
		constexpr float fltMin = std::numeric_limits<float>::lowest();
		constexpr float fltMax = std::numeric_limits<float>::max();
		constexpr int intMin = std::numeric_limits<int>::lowest();
		constexpr int intMax = std::numeric_limits<int>::max();
	}

	struct EditResult
	{
		bool changed = false;		// value changed this frame
		bool active = false;		// currently being edited
		bool started = false;		// first frame of interaction
		bool finished = false;	// released after edit

		EditResult& operator |= (const EditResult& other)
		{
			changed |= other.changed;
			active |= other.active;
			started |= other.started;
			finished |= other.finished;
			return *this;
		}
	};

	struct PropertyTable
	{
		bool open = false;

		PropertyTable(float labelWidth = 100.0f)
		{
			open = ImGui::BeginTable("table", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoBordersInBody);

			if (open)
			{
				ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, labelWidth);
				ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
			}
		}

		~PropertyTable()
		{
			if (open) { ImGui::EndTable(); }
		}

		explicit operator bool() const { return open; }
	};

	struct PropertyRow
	{
		PropertyRow(const char* label)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted(label);
			ImGui::TableSetColumnIndex(1);
		}
	};

	bool CheckAssetType(Engine::Uuid id, Engine::AssetType expectedType);

	struct DragDropTarget
	{
		DragDropTarget()
		{
			active = ImGui::BeginDragDropTarget();
		}

		~DragDropTarget()
		{
			if (active) ImGui::EndDragDropTarget();
		}

		template<Engine::AssetType ExpectedType, typename Fn>
		void AcceptAsset(Fn&& fn)
		{
			static_assert(std::is_invocable_v<Fn, Engine::Uuid>, "Callback must take an Engine::Uuid");

			if (!active) return;

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UUID"))
			{
				assert(payload->DataSize == sizeof(Engine::Uuid) && "Drag drop payload size mismatch!");
				Engine::Uuid id = *static_cast<const Engine::Uuid*>(payload->Data);

				if (CheckAssetType(id, ExpectedType))
				{
					std::forward<Fn>(fn)(id);
				}
			}
		}

	private:
		bool active = false;
	};


	EditResult TextureField(const std::string& label, Engine::Uuid& textureId);
	EditResult AudioClipField(const std::string& label, Engine::AudioClip*& audioClip);
	EditResult MeshField(const std::string& label, Engine::Mesh*& mesh);
	EditResult ShaderField(const std::string& label, Engine::Shader*& shader);
	EditResult MaterialField(const std::string& label, Engine::Material*& material);
	EditResult IntField(const std::string& label, int& value, int min = intMin, int max = intMax);
	EditResult FloatField(const std::string& label, float& value, float min = fltMin, float max = fltMax, float speed = 0.05f);
	EditResult FloatSliderField(const std::string& label, float& value, float min = 0, float max = 1);
	EditResult Vec2Field(const std::string& label, glm::vec2& value);
	EditResult Vec3Field(const std::string& label, glm::vec3& value, float min = fltMin, float max = fltMax, float speed = 0.01f);
	EditResult BoolField(const std::string& label, bool& value);
	EditResult ColorField(const std::string& label, glm::vec4& color);
	EditResult ColorField(const std::string& label, glm::vec3& color);
	EditResult EnumField(const std::string& label, int& value, const std::vector<std::string>& options);
	EditResult TextField(const std::string& label, std::string& text);
}