#pragma once
#include "Engine/Renderer/Texture.h"
#include "Engine/Core/Core.h"
#include <glm/glm.hpp>
#include <string>

namespace Editor::UI
{
	bool TextureField(const char* label, Ref<Engine::Texture2D>& texture);
	bool IntField(const char* label, int& value);
	bool FloatField(const char* label, float& value);
	bool FloatSliderField(const char* label, float& value, float min = 0.0f, float max = 1.0f);
	bool Vec2Field(const char* label, glm::vec2& values);
	bool Vec3Field(const char* label, glm::vec3& values);
	bool ColorField(const char* label, glm::vec4& color);
	bool ColorField(const char* label, glm::vec3& color);

	bool EnumField(const char* label, int& value, const std::vector<std::string>& options);
	bool EnumField(const char* label, int& value, const std::vector<std::filesystem::path>& options);


	// --- deprecated ---
	void StringControl(const char* label, std::string& value, const char* resetValue = "");

}