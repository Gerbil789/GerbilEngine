#pragma once

#include "Engine/Renderer/Texture.h"
#include <glad/glad.h>

namespace Engine 
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual void SetData(void* data, uint32_t size) override;
		virtual void Bind(uint32_t slot = 0) const override;

		void Load(const std::string& filePath) override;
		void Unload() override;
	private:
		GLenum m_InternalFormat;
		GLenum m_DataFormat;
	};
}

