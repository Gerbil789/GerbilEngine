#include "enginepch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Engine 
{
	static const uint32_t s_MaxFramebufferSize = 8192;

	static bool IsDepthFormat(FrameBufferTextureFormat format)
	{
		switch (format)
		{
		case FrameBufferTextureFormat::DEPTH24STENCIL8:
			return true;
		default:
			return false;
		}
	}

	static void BindTexture(bool multisample, uint32_t id)
	{
		glBindTexture(multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, id);
	}

	static void CreateTextures(bool multisample, uint32_t* outID, uint32_t count)
	{
		glCreateTextures(multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, count, outID);
	}

	static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
	{
		bool multisample = samples > 1;
		if (multisample)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, id, 0);
	}

	static void AttachDepthTexture(uint32_t id, int samples, GLenum format, uint32_t width, uint32_t height)
	{
		bool multisample = samples > 1;
		if (multisample)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
		}
		else
		{
			glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, id, 0);
		}
	}

	static GLenum EngineTextureFormatToGL(FrameBufferTextureFormat format)
	{
		switch (format)
		{
		case FrameBufferTextureFormat::RGBA8:
			return GL_RGBA8;
		case FrameBufferTextureFormat::RGBA16F:
			return GL_RGBA16F;
		case FrameBufferTextureFormat::RGBA32F:
			return GL_RGBA32F;
		case FrameBufferTextureFormat::RED_INTEGER:
			return GL_RED_INTEGER;
		default:
			ENGINE_LOG_ERROR("Unknown Texture Format!");
			return 0;
		}
	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification & spec) : m_Specification(spec)
	{
		for(auto format : m_Specification.Attachments.Attachments)
		{
			if (!IsDepthFormat(format.TextureFormat))
				m_ColorAttachmentSpecifications.emplace_back(format);
			else
				m_DepthAttachmentSpecification = format;
		}

		Invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachmentSpecifications.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}



	void OpenGLFrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);

		int value = -1;
		if(m_ColorAttachments.size() > 1)
		{
			glClearTexImage(m_ColorAttachments[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, &value);
		}
	}

	void OpenGLFrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			ENGINE_LOG_WARNING("Attempted to resize framebuffer to {0}, {1}", width, height);
			return;
		}

		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();

	}
	void OpenGLFrameBuffer::Invalidate()
	{
		if (m_RendererID) {
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachmentSpecifications.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisample = m_Specification.Samples > 1;

		//color attachments
		if(m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++) 
			{
				BindTexture(multisample, m_ColorAttachments[i]);

				switch(m_ColorAttachmentSpecifications[i].TextureFormat)
				{
					case FrameBufferTextureFormat::RGBA8:
					{
						AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
						break;
					}
					case FrameBufferTextureFormat::RGBA16F:
					{
						AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA16F, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
						break;
					}
					case FrameBufferTextureFormat::RGBA32F:
					{
						AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA32F, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
						break;
					}
					case FrameBufferTextureFormat::RED_INTEGER:
					{
						AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
						break;
					}
					default:
					{
						ENGINE_LOG_ERROR("Unknown Texture Format!");
						break;
					}
				}
			}
		}

		//depth attachment
		if(m_DepthAttachmentSpecification.TextureFormat != FrameBufferTextureFormat::None)
		{
			CreateTextures(multisample, &m_DepthAttachment, 1);
			BindTexture(multisample, m_DepthAttachment);

			switch(m_DepthAttachmentSpecification.TextureFormat)
			{
				case FrameBufferTextureFormat::DEPTH24STENCIL8:
				{
					AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
					break;
				}
				default:
				{
					ENGINE_LOG_ERROR("Unknown Texture Format!");
					break;
				}
			}
		}

		if (m_ColorAttachments.size() > 1)
		{
			ASSERT(m_ColorAttachments.size() <= 4, "Framebuffer only supports 4 attachments!");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			glDrawBuffer(GL_NONE); // Don't draw to any attachments (depth-only)
		}

		ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		ASSERT(attachmentIndex < m_ColorAttachments.size(), "attachmentIndex is bigger than color attachment size");

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		ASSERT(attachmentIndex < m_ColorAttachments.size(), "attachmentIndex is bigger than color attachment size");

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		glClearTexImage(m_ColorAttachments[attachmentIndex], 0, EngineTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
	}
}