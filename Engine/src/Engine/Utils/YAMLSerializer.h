#pragma once

#include <filesystem>
#include <glm/glm.hpp>

namespace Engine
{
	class YamlSerializer 
	{
	public:
		// === Writing ===
		void BeginMap(const std::string& name = {});
		void EndMap();

		void BeginSequence(const std::string& name);
		void EndSequence();

		template<typename T>
		void Write(const std::string& key, const T& value) {
			if (!m_Stack.empty())
				m_Stack.back().lines.push_back(key + ": " + SerializeValue(value));
			else
				m_FinalLines.push_back(key + ": " + SerializeValue(value));
		}

		void SaveToFile(const std::filesystem::path& filepath) const;

		// === Reading ===
		//bool LoadFromFile(const std::filesystem::path& filepath);
		//bool BeginMapRead(const std::string& key = {});
		//void EndMapRead();

		//bool BeginSequenceRead(const std::string& key);
		//bool NextSequenceItem();
		//void EndSequenceRead();

		//template<typename T>
		//T Read(const std::string& key, const T& defaultValue = T()) const;


	private:
		inline std::string SerializeValue(int value) {
			return std::to_string(value);
		}

		inline std::string SerializeValue(float value) {
			return std::to_string(value);
		}

		inline std::string SerializeValue(double value) {
			return std::to_string(value);
		}

		inline std::string SerializeValue(bool value) {
			return value ? "true" : "false";
		}

		inline std::string SerializeValue(const std::string& value) {
			return "\"" + value + "\""; // quoted
		}

		inline std::string SerializeValue(const glm::vec2& v) {
			std::ostringstream ss;
			ss << "[" << v.x << ", " << v.y << "]";
			return ss.str();
		}

		inline std::string SerializeValue(const glm::vec3& v) {
			std::ostringstream ss;
			ss << "[" << v.x << ", " << v.y << ", " << v.z << "]";
			return ss.str();
		}

		inline std::string SerializeValue(const glm::vec4& v) {
			std::ostringstream ss;
			ss << "[" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]";
			return ss.str();
		}

	private:
		enum class ContextType { Map, Sequence };

		struct Context {
			ContextType type;
			std::string key;
			std::vector<std::string> lines;
		};

		std::vector<Context> m_Stack;
		std::vector<std::string> m_FinalLines;
		bool m_Loading = false;
		std::unordered_map<std::string, std::string> m_LoadedValues;

		void FlushCurrentContext();
	};
}