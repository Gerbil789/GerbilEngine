#pragma once

#include "Engine/Core/Asset.h"
#include "Engine/Renderer/Buffer.h"

namespace Engine 
{
	class Shader : public Asset
	{
	public:
		Shader(const std::filesystem::path& path) : Asset(path) {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual const BufferLayout& GetUniformBuffer() const = 0;

	};

	class ShaderFactory : public IAssetFactory
	{
	public:
		virtual Ref<Asset> Load(const std::filesystem::path& path, const std::any& data) override;
		virtual Ref<Asset> Create(const std::filesystem::path& path, const std::any& data) { return nullptr; }
	};
}