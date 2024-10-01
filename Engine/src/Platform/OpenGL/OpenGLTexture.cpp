#include "enginepch.h"
#include "OpenGLTexture.h"
#include "stb_image.h"

namespace Engine 
{
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height) : Texture2D("none")
	{
		ENGINE_PROFILE_FUNCTION();
		m_Width = width;
		m_Height = height;
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path& path, GLenum format) : Texture2D(path)
	{
		ENGINE_PROFILE_FUNCTION();
		int width, height, channels;
		stbi_set_flip_vertically_on_load(!format);

		stbi_uc* data = nullptr;
		{
			ENGINE_PROFILE_SCOPE("OpenGLTexture2D::OpenGLTexture2D - stbi_load");
			data = stbi_load(path.string().c_str(), &width, &height, &channels, format);
		}

		ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;


		if (format != 0) 
		{
			m_PixelData = new stbi_uc[width * height * channels];
			std::memcpy(m_PixelData, data, width * height * channels);
		}
		


		switch (channels)
		{
		case 4:
			m_InternalFormat = GL_RGBA8;
			m_DataFormat = GL_RGBA;
			break;
		case 3:
			m_InternalFormat = GL_RGB8;
			m_DataFormat = GL_RGB;
			break;
		case 2:
			m_InternalFormat = GL_RG8;
			m_DataFormat = GL_RG;
			break;
		case 1:
			m_InternalFormat = GL_R8;
			m_DataFormat = GL_RED;
			break;
		default:
			ASSERT(false, "Format not supported!");
			break;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		ENGINE_PROFILE_FUNCTION();
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		ENGINE_PROFILE_FUNCTION();
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;

		ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		ENGINE_PROFILE_FUNCTION();
		glBindTextureUnit(slot, m_RendererID);
	}
}