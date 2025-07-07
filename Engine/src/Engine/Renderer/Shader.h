#pragma once

#include "Engine/Core/Asset.h"
#include "Engine/Renderer/Buffer.h"

namespace Engine 
{
	class Shader : public Asset
	{
	public:
		Shader(const std::filesystem::path& path) : Asset(path) {}

		void Bind() const {}
		void Unbind() const {}

		void SetInt(const std::string& name, int value);
		void SetIntArray(const std::string& name, int* values, uint32_t count);
		void SetFloat(const std::string& name, float value);
		void SetFloat3(const std::string& name, const glm::vec3& value);
		void SetFloat4(const std::string& name, const glm::vec4& value);
		void SetMat4(const std::string& name, const glm::mat4& value);

		const BufferLayout& GetInputBufferLayout() const { return m_InputLayout; }
		BufferLayout GetMaterialBufferLayout() const { return m_MaterialLayout; }

		//const DynamicVertex& GetVertex() const { return m_Vertex; }

	protected:
		BufferLayout m_InputLayout = BufferLayout();		//shader inputs
		BufferLayout m_MaterialLayout = BufferLayout(); //shader material uniforms
		//DynamicVertex m_Vertex;													//vertex data
	};

	class ShaderFactory : public IAssetFactory
	{
	public:
		virtual Ref<Asset> Load(const std::filesystem::path& path, const std::any& data) override;
		virtual Ref<Asset> Create(const std::filesystem::path& path, const std::any& data) { return nullptr; }
	};
}