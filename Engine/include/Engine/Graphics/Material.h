#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Graphics/SamplerPool.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/Pipeline.h"
#include <variant>
#include <glm/glm.hpp>

namespace Engine
{
	using MaterialValue = std::variant<float, glm::vec2, glm::vec3, glm::vec4, glm::ivec2>;

	struct MaterialSpecification 
	{
		Uuid shaderId;

		std::unordered_map<std::string, MaterialValue> parameters;
		std::unordered_map<std::string, Uuid> textures;

		TextureFilter filter = TextureFilter::Bilinear;
		TextureWrap wrap = TextureWrap::Repeat;
	};

	class ENGINE_API Material : public Asset
	{
	public:
		Material(const MaterialSpecification& spec);

		const Uuid GetShader() const { return m_ShaderId; }
		void SetShader(Uuid shaderId);

		void SetTextureFilter(TextureFilter filter) { m_TextureFilter = filter; CreateBindGroup(); }
		void SetTextureWrap(TextureWrap wrap) { m_TextureWrap = wrap; CreateBindGroup(); }

		TextureFilter GetTextureFilter() const { return m_TextureFilter; }
		TextureWrap GetTextureWrap() const { return m_TextureWrap; }

		void SetTexture(const std::string& name, Uuid texture);
		Uuid GetTexture(const std::string& name) const;

		wgpu::BindGroup GetBindGroup() const { return m_BindGroup; }
		wgpu::Buffer GetUniformBuffer() const { return m_UniformBuffer; }
		const std::vector<std::byte>& GetUniformData() const { return m_UniformData; }
		const std::unordered_map<std::string, Uuid>& GetTextures() const { return m_Textures; }
		const std::unordered_map<std::string, MaterialValue>& GetParameters() const { return m_Parameters; }

		const PipelineSpecification& GetPipelineSpec() const { return m_PipelineSpec; }

		template<typename T>
		void SetParameter(const std::string& paramName, const T& value);

		const MaterialValue& GetParameterVariant(const std::string& name) const;

	private:
		void CreateUniformBuffer();
		void CreateBindGroup();

	private:
		Uuid m_ShaderId;
		std::vector<std::byte> m_UniformData; // parameters data packed according to shader layout (material uniform layout)
		std::unordered_map<std::string, Uuid> m_Textures;
		wgpu::BindGroup m_BindGroup;
		wgpu::Buffer m_UniformBuffer; 

		TextureFilter m_TextureFilter = TextureFilter::Bilinear;
		TextureWrap m_TextureWrap = TextureWrap::Repeat;

		PipelineSpecification m_PipelineSpec;

		std::unordered_map<std::string, MaterialValue> m_Parameters;
	};
}