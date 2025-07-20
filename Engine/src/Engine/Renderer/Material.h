#pragma once

#include "Engine/Core/Asset.h"
#include "Engine/Renderer/Shaders/Shader.h"
#include "Engine/Renderer/Texture.h"
#include <glm/glm.hpp>
#include <variant>

namespace Engine 
{
	using MaterialValue = std::variant<float, glm::vec3, glm::vec4>;

	class Material : public Asset
	{
	public:
		Material(const std::filesystem::path& path) : Asset(path) {}

		void SetValue(const std::string& name, MaterialValue value) 
		{
			m_Values[name] = std::move(value);
		}

		const MaterialValue* GetValue(const std::string& name) const 
		{
			auto it = m_Values.find(name);
			return (it != m_Values.end()) ? &it->second : nullptr;
		}

		void SetTexture(const std::string& name, const Ref<Texture2D>& texture) 
		{
			m_Textures[name] = texture;
		}

		Ref<Shader> GetShader() { return m_Shader; }
		void SetShader(const Ref<Shader>& shader);

		wgpu::BindGroup GetMaterialBindGroup() const { return m_MaterialBindGroup; }
		wgpu::Buffer GetMaterialUniformBuffer() const { return m_MaterialUniformBuffer; }

		void Bind(wgpu::RenderPassEncoder pass);

	private:
		void CreateMaterialUniformBuffer();
		void CreateMaterialBindGroup();

	private:
		Ref<Shader> m_Shader;

		wgpu::BindGroup m_MaterialBindGroup;

		std::unordered_map<std::string, MaterialValue> m_Values;
		std::vector<uint8_t> m_UniformData;

		wgpu::Buffer m_MaterialUniformBuffer;

		std::unordered_map<std::string, Ref<Texture2D>> m_Textures;

	};
}