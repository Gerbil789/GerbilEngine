#pragma once
#include <webgpu/webgpu.hpp>

namespace Engine
{
	class GraphicsContext
	{
	public:
		void Init();
		void Shutdown();
		void SetViewport(uint32_t width, uint32_t height);

		wgpu::Device GetDevice() const { return m_Device; }
		wgpu::Queue GetQueue() const { return m_Queue; }
		wgpu::Surface GetSurface() const { return m_Surface; }

	private:
		wgpu::Device m_Device;
		wgpu::Queue m_Queue;
		wgpu::Surface m_Surface;
	};
}