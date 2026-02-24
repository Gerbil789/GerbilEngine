#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Graphics/SamplerPool.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/Texture.h"

namespace Engine 
{
	struct MaterialSpecification
	{
		Shader* shader = nullptr;

		TextureFilter filter = TextureFilter::Bilinear;
		TextureWrap wrap = TextureWrap::Repeat;

		std::unordered_map<std::string, float> floatDefaults;
		std::unordered_map<std::string, glm::vec4> vec4Defaults;
		std::unordered_map<std::string, Uuid> textureDefaults;
	};

	class ENGINE_API Material : public Asset
	{
	public:
		Material(const MaterialSpecification& spec);

		Shader* GetShader() const { return m_Shader; }
		void SetShader(Shader* shader);

		void SetFloat(const std::string& paramName, float value);
		void SetVec4(const std::string& paramName, const glm::vec4& value);

		void SetTextureFilter(TextureFilter filter) { m_TextureFilter = filter; CreateBindGroup(); }
		void SetTextureWrap(TextureWrap wrap) { m_TextureWrap = wrap; CreateBindGroup(); }

		TextureFilter GetTextureFilter() const { return m_TextureFilter; }
		TextureWrap GetTextureWrap() const { return m_TextureWrap; }

		void SetTexture(const std::string& name, Texture2D* texture);
		Texture2D* GetTexture(const std::string& name) const;

		wgpu::BindGroup GetBindGroup() const { return m_BindGroup; }
		wgpu::Buffer GetUniformBuffer() const { return m_UniformBuffer; }
		const std::vector<std::byte>& GetUniformData() const { return m_UniformData; }
		const std::unordered_map<std::string, Texture2D*>& GetTextures() const { return m_Textures; }

	private:
		void CreateUniformBuffer();
		void CreateBindGroup();

	private:
		Shader* m_Shader;

		std::vector<std::byte> m_UniformData; // parameters data packed according to shader layout
		std::unordered_map<std::string, Texture2D*> m_Textures; // this is for engine management, the webgpu binds it once and does not need it afterwards
		wgpu::BindGroup m_BindGroup;
		wgpu::Buffer m_UniformBuffer; 

		TextureFilter m_TextureFilter = TextureFilter::Bilinear;
		TextureWrap m_TextureWrap = TextureWrap::Repeat;
	};
}

namespace Engine::Materials
{
	ENGINE_API Material* GetDefault();
	ENGINE_API Material* CreateMaterial(const std::filesystem::path& path, const std::string& name);
}