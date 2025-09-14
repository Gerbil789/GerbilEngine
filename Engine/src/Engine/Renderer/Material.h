#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Texture.h"
#include <glm/glm.hpp>
#include <variant>

namespace Engine 
{
	class Material : public Asset
	{
	public:
		Material() = default; //TODO: remove?
		Material(const Ref<Shader>& shader);

		void SetTexture(const std::string& name, Ref<Texture2D> texture);

		Ref<Shader> GetShader() const { return m_Shader; }

		template<typename T>
		void Set(const std::string& name, const T& value)
		{
			//const auto* info = m_Shader->GetParamInfo(name);
			//if (!info) return;

			//memcpy(m_UniformData.data() + info->offset, &value, info->size);
		}

		void Bind(wgpu::RenderPassEncoder pass);
	private:
		void CreateSampler();
		void CreateUniformBuffer(size_t size);
		void CreateBindGroup();

	private:
		Ref<Shader> m_Shader;

		std::vector<uint8_t> m_UniformData; // paramerers data
		std::unordered_map<std::string, Ref<Texture2D>> m_Textures;
		wgpu::Sampler m_Sampler;

		wgpu::BindGroup m_BindGroup;
		wgpu::Buffer m_UniformBuffer; 
	};
}