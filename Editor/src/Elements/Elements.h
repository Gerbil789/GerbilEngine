#pragma once

#include <glm/glm.hpp>
#include <string>

namespace Engine
{
	namespace UI 
	{
		const int labelWidth = 100;

		void IntControl(const char* label, int& value, int resetValue = 0);
		void Vec2IntControl(const char* label, glm::ivec2& values, int resetValue = 0);
		void Vec3IntControl(const char* label, glm::ivec3& values, int resetValue = 0);
		void Vec4IntControl(const char* label, glm::ivec4& values, int resetValue = 0);

		void FloatControl(const char* label, float& value, float resetValue = 0.0f);
		void Vec2Control(const char* label, glm::vec2& values, float resetValue = 0.0f);
		void Vec3Control(const char* label, glm::vec3& values, float resetValue = 0.0f);
		void Vec4Control(const char* label, glm::vec4& values, float resetValue = 0.0f);

		void BoolControl(const char* label, bool& value);
		void Vec2BoolControl(const char* label, glm::bvec2& values);
		void Vec3BoolControl(const char* label, glm::bvec3& values);
		void Vec4BoolControl(const char* label, glm::bvec4& values);

		void StringControl(const char* label, std::string& value, const char* resetValue = "");
	}
}