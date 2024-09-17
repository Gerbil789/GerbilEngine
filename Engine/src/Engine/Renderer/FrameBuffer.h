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
		virtual ~FrameBuffer() = default;

		virtual const FrameBufferSpecification& GetSpecification() const = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		//virtual uint32_t GetDepthAttachmentRendererID() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);

		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;
	};
}

