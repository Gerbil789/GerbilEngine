#pragma once

#include "Engine/Core/Asset.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Engine 
{
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

		Ref<Shader> GetShader() { return m_Shader; }

		void SetShader(const Ref<Shader>& shader);


	private:
		Ref<Shader> m_Shader;
	};
}