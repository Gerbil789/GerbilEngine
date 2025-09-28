#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Texture.h"
#include <glm/glm.hpp>

namespace Engine 
{
	class Material : public Asset
	{
		friend class MaterialSerializer;

	public:
		Material(const Ref<Shader>& shader);

		Ref<Shader> GetShader() const { return m_Shader; }

		void SetFloat(const std::string& paramName, float value);
		//void SetVec2(const std::string& paramName, const glm::vec2& value);
		//void SetVec3(const std::string& paramName, const glm::vec3& value);
		void SetVec4(const std::string& paramName, const glm::vec4& value);
		void SetTexture(const std::string& name, Ref<Texture2D> texture);

		void Bind(wgpu::RenderPassEncoder pass);
		std::vector<uint8_t> GetUniformData() const { return m_UniformData; } //TODO: this should not be public
	private:
		void CreateSampler();
		void CreateUniformBuffer();
		void CreateBindGroup();

	private:
		Ref<Shader> m_Shader;

		std::vector<uint8_t> m_UniformData; // paramerers data packed according to shader layout
		std::unordered_map<std::string, Ref<Texture2D>> m_Textures; // this is for engine management, the webgpu binds it once and does not need it afterwards
		wgpu::Sampler m_Sampler;
		wgpu::BindGroup m_BindGroup;
		wgpu::Buffer m_UniformBuffer; 
	};
}