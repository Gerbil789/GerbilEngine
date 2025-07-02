#pragma once

#include "Engine/Core/Core.h"

namespace Engine
{
	enum class FrameBufferTextureFormat
	{
		None = 0,
		RGBA8,
		RGBA16F,
		RGBA32F,
		DEPTH24STENCIL8,
		RED_INTEGER
	};

	struct FrameBufferTextureSpecification 
	{
		FrameBufferTextureSpecification() = default;
		FrameBufferTextureSpecification(FrameBufferTextureFormat format) : TextureFormat(format) {}

		FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::None;
		//TODO: filtering/wrap
	};

	struct FrameBufferAttachmentSpecification
	{
		FrameBufferAttachmentSpecification() = default;
		FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferTextureSpecification> attachments): Attachments(attachments) {}

		std::vector<FrameBufferTextureSpecification> Attachments;
	};

	struct FrameBufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		FrameBufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;
		bool SwapChainTarget = false;
	};


	class FrameBuffer
	{
	public:
		FrameBuffer(const FrameBufferSpecification& specification) : m_Specification(specification) {}
		~FrameBuffer() = default;

		const FrameBufferSpecification& GetSpecification() const { return m_Specification; }

		void Bind() {}
		void Unbind() {}

		void Resize(uint32_t width, uint32_t height) {}

		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const { return m_ColorAttachments[index]; }
		//virtual uint32_t GetDepthAttachmentRendererID() const = 0;

		uint32_t GetWidth() const { return m_Specification.Width; }
		uint32_t GetHeight() const { return m_Specification.Height; }


		int ReadPixel(uint32_t attachmentIndex, int x, int y);

		void ClearAttachment(uint32_t attachmentIndex, int value);

	private:
		FrameBufferSpecification m_Specification;
		uint32_t m_RendererID = 0;
		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}

