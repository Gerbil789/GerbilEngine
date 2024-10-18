#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/Asset.h"

namespace Engine
{
	class Texture2DFactory : public IAssetFactory
	{
	public:
		virtual Ref<Asset> Load(const std::filesystem::path& path, const std::any& data = std::any()) override;
		virtual Ref<Asset> Create(const std::filesystem::path& path, const std::any& data = std::any()) override;

		Ref<Asset> CreateTexture(uint32_t width, uint32_t height, uint32_t data);
		Ref<Asset> CreateSolidColorTexture(uint32_t color); //ABGR
	};

	class Texture2D : public Asset
	{
	public:
		virtual ~Texture2D() = default;

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		uint32_t GetRendererID() const { return m_RendererID; }
		unsigned char* GetPixelData() const { return m_PixelData; }

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool operator == (const Texture2D& other) const { return m_RendererID == other.GetRendererID(); }

	protected:
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		unsigned char* m_PixelData = nullptr;

		Texture2D(const std::filesystem::path& path) : Asset(path) {}
	};
}