#pragma once

#include "Engine/Asset/Asset.h"
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

		const std::map<std::string, MaterialValue>& GetValues() const { return m_Values; }
		const std::map<std::string, Ref<Texture2D>>& GetTextures() const { return m_Textures; }

		wgpu::BindGroup GetMaterialBindGroup() const { return m_MaterialBindGroup; }
		wgpu::Buffer GetMaterialUniformBuffer() const { return m_MaterialUniformBuffer; }

		void Bind(wgpu::RenderPassEncoder pass);

	private:
		void CreateMaterialUniformBuffer();
		void CreateMaterialBindGroup();

	private:
		Ref<Shader> m_Shader;

		wgpu::BindGroup m_MaterialBindGroup;
		wgpu::Buffer m_MaterialUniformBuffer;

		std::map<std::string, MaterialValue> m_Values;
		std::map<std::string, Ref<Texture2D>> m_Textures;
		std::vector<uint8_t> m_UniformData;
	};
}