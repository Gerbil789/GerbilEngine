#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/SamplerPool.h"
#include <glm/glm.hpp>

namespace Engine 
{
	class Material : public Asset
	{
		friend class MaterialSerializer;

	public:
		Material(Shader* shader);

		Shader* GetShader() const { return m_Shader; }
		void SetShader(Shader* shader);

		void SetFloat(const std::string& paramName, float value);
		//void SetVec2(const std::string& paramName, const glm::vec2& value);
		//void SetVec3(const std::string& paramName, const glm::vec3& value);
		void SetVec4(const std::string& paramName, const glm::vec4& value);

		void SetTextureFilter(TextureFilter filter) { m_TextureFilter = filter; CreateBindGroup(); }
		void SetTextureWrap(TextureWrap wrap) { m_TextureWrap = wrap; CreateBindGroup(); }

		TextureFilter GetTextureFilter() const { return m_TextureFilter; }
		TextureWrap GetTextureWrap() const { return m_TextureWrap; }

		void SetTexture(const std::string& name, Ref<Texture2D> texture);
		Ref<Texture2D> GetTexture(const std::string& name) const
		{
			auto it = m_Textures.find(name);
			if (it != m_Textures.end())
			{
				return it->second;
			}
			return nullptr;
		}

		void Bind(wgpu::RenderPassEncoder pass);
		std::vector<uint8_t> GetUniformData() const { return m_UniformData; } //TODO: this should not be public

		static Ref<Material> GetDefault();

	private:
		void CreateUniformBuffer();
		void CreateBindGroup();

	private:
		Shader* m_Shader;

		std::vector<uint8_t> m_UniformData; // paramerers data packed according to shader layout
		std::unordered_map<std::string, Ref<Texture2D>> m_Textures; // this is for engine management, the webgpu binds it once and does not need it afterwards
		wgpu::BindGroup m_BindGroup;
		wgpu::Buffer m_UniformBuffer; 

		TextureFilter m_TextureFilter = TextureFilter::Bilinear;
		TextureWrap m_TextureWrap = TextureWrap::Repeat;


		inline static Ref<Material> s_DefaultMaterial;

	};
}