#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/Asset.h"

namespace Engine
{
	class Texture2DFactory : public IAssetFactory
	{
	public:
		virtual Ref<Asset> Load(const std::string& filePath) override;
		virtual Ref<Asset> Create(const std::string& filePath) override;
	};

	class Texture2D : public Asset
	{
	public:
		virtual ~Texture2D() = default;

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		uint32_t GetRendererID() const { return m_RendererID; }

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool operator == (const Texture2D& other) const { return m_RendererID == other.GetRendererID(); }

		static Ref<Texture2D> Create(uint32_t width, uint32_t height); //TODO: what is this? move it to the factory
	protected:
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;

		Texture2D(const std::string& filePath) : Asset(filePath) {}
	};
}