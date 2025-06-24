#pragma once

#include <unordered_map>
#include <typeindex>

namespace Editor
{
	class EditorServiceRegistry {
	public:
		template<typename T>
		static void Register(T* instance) {
			s_Services[typeid(T)] = instance;
		}

		template<typename T>
		static T* Get() {
			auto it = s_Services.find(typeid(T));
			if (it != s_Services.end())
				return static_cast<T*>(it->second);
			return nullptr;
		}

	private:
		static inline std::unordered_map<std::type_index, void*> s_Services;
	};
}