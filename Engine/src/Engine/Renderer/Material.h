#pragma once

#include "Engine/Core/Asset.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Texture.h"
#include <glm/glm.hpp>
#include <variant>

namespace Engine 
{
	using MaterialProperty = std::variant<float, int, glm::vec2, glm::vec3, glm::vec4, std::shared_ptr<Texture2D>>;

	class MaterialFactory : public IAssetFactory
	{
	public:
		Ref<Asset> Load(const std::filesystem::path& path, const std::any& data = std::any()) override;
		Ref<Asset> Create(const std::filesystem::path& path, const std::any& data = std::any()) override;
	};

	class Material : public Asset
	{
	public:
		Material(const std::filesystem::path& path) : Asset(path) {}

		const Ref<Shader>& GetShader() const { return m_Shader; }
		void SetShader(const Ref<Shader>& shader);

		void SetProperties();

		template<typename T>
		void SetProperty(const std::string& name, const T& value)
		{
			m_Properties[name] = value;
		}

		template<typename T>
		T GetProperty(const std::string& name) const
		{
			if (m_Properties.find(name) != m_Properties.end())
			{
				return std::get<T>(m_Properties.at(name));
			}
			LOG_ERROR("Property {0} not found", name);
			return T();
		}

		bool HasProperty(const std::string& name) const
		{
			return m_Properties.find(name) != m_Properties.end();
		}

		void ClearProperties()
		{
			m_Properties.clear();
		}

		std::unordered_map<std::string, MaterialProperty> GetProperties()
		{
			return m_Properties;
		}

	private:
		Ref<Shader> m_Shader;
		std::unordered_map<std::string, MaterialProperty> m_Properties;

	};
}