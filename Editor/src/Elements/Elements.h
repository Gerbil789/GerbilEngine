#pragma once
#include "Engine/Renderer/Texture.h"
#include <glm/glm.hpp>
#include <string>

//WARNING: This file is under construction and it contains a lot of trash code

namespace Engine::UI
{
	const int labelWidth = 100;

	bool TextureField(const char* label, Ref<Texture2D>& texture);
	bool FloatField(const char* label, float& value);
	bool FloatSliderField(const char* label, float& value, float min = 0.0f, float max = 1.0f);
	bool Vec2Field(const char* label, glm::vec2& values);
	bool ColorField(const char* label, glm::vec4& color);
	bool ColorField(const char* label, glm::vec3& color);



	// --- deprecated ---
	void IntControl(const char* label, int& value, int resetValue = 0);
	void Vec2IntControl(const char* label, glm::ivec2& values, int resetValue = 0);
	void Vec3IntControl(const char* label, glm::ivec3& values, int resetValue = 0);
	void Vec4IntControl(const char* label, glm::ivec4& values, int resetValue = 0);
	void Vec3Control(const char* label, glm::vec3& values, float resetValue = 0.0f);
	void Vec4Control(const char* label, glm::vec4& values, float resetValue = 0.0f);
	void BoolControl(const char* label, bool& value);
	void Vec2BoolControl(const char* label, glm::bvec2& values);
	void Vec3BoolControl(const char* label, glm::bvec3& values);
	void Vec4BoolControl(const char* label, glm::bvec4& values);
	bool FloatSliderControl(const char* label, float& value, float min = 0.0f, float max = 1.0f);
	void StringControl(const char* label, std::string& value, const char* resetValue = "");
	bool EnumControl(const char* label, int& value, const std::vector<std::string>& options);


}