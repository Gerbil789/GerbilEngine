#include "enginepch.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Renderer.h"

namespace Engine
{
	Ref<Asset> Texture2DFactory::Load(const std::filesystem::path& path, const std::any& data)
	{
		int format = 0;
		if (data.has_value()) 
		{
			try
			{
				format = std::any_cast<int>(data);
			}
			catch (const std::bad_any_cast&)
			{
				LOG_WARNING("Failed to cast 'data' to GLenum. Using default settings.");
			}
		}

		//return CreateRef<Texture2D>(path);
		return CreateRef<Texture2D>(path, format);
		
	}

	Ref<Asset> Texture2DFactory::Create(const std::filesystem::path& path, const std::any& data)
	{
		return CreateRef<Texture2D>(path);

	}

	Ref<Asset> Texture2DFactory::CreateTexture(uint32_t width, uint32_t height, uint32_t data)
	{
		Ref<Texture2D> texture = CreateRef<Texture2D>(width, height);
		texture->SetData(&data, sizeof(uint32_t));
		return texture;

	}

	Ref<Asset> Texture2DFactory::CreateSolidColorTexture(uint32_t color)
	{
		Ref<Texture2D> texture = CreateRef<Texture2D>(1, 1);
		std::vector<uint32_t> colorData(1, color);
		texture->SetData(colorData.data(), sizeof(uint32_t));
		return texture;
	}
}