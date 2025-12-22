#pragma once

#include "Engine/Renderer/Texture.h"
#include <string>
#include <glm/glm.hpp>

namespace Editor
{
	struct WidgetResult
	{
		bool changed = false;		// value changed this frame
		bool active = false;		// currently being edited
		bool started = false;		// first frame of interaction
		bool finished = false;	// released after edit

		WidgetResult& operator |= (const WidgetResult& other)
		{
			changed |= other.changed;
			active |= other.active;
			started |= other.started;
			finished |= other.finished;
			return *this;
		}
	};
}

namespace Editor::Widget
{
	WidgetResult TextureField(const char* label, Engine::Texture2D*& texture);
	WidgetResult IntField(const char* label, int& value);
	WidgetResult FloatField(const char* label, float& value);
	WidgetResult FloatSliderField(const char* label, float& value, float min = 0.0f, float max = 1.0f);
	WidgetResult Vec2Field(const char* label, glm::vec2& value);
	WidgetResult Vec3Field(const char* label, glm::vec3& value);
	WidgetResult ColorField(const char* label, glm::vec4& color);
	WidgetResult ColorField(const char* label, glm::vec3& color);
	WidgetResult EnumField(const char* label, int& value, const std::vector<std::string>& options);
	WidgetResult EnumField(const char* label, int& value, const std::vector<std::filesystem::path>& options);
}