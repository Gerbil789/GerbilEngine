#pragma once

#include "Engine/Renderer/FrameBuffer.h"

namespace Engine {
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FrameBufferSpecification & spec);
		virtual ~OpenGLFrameBuffer();

		virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		void Invalidate();

		virtual uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; }
		virtual uint32_t GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
		uint32_t m_Width = 0, m_Height = 0;
		FrameBufferSpecification m_Specification;

	};


}

