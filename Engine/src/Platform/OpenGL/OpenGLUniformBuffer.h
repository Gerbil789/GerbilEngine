#pragma once

#include "Engine/Renderer/UniformBuffer.h"

namespace Engine {

	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer();

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
		virtual void Bind(uint32_t binding) const override;
	private:
		uint32_t m_RendererID = 0;
	};
}