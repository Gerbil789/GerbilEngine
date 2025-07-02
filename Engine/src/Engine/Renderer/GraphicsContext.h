#pragma once

#include <webgpu/webgpu.h>

namespace Engine
{
	class GraphicsContext
	{
	public:
		void Init();
		void Shutdown();
		void SwapBuffers() {}

		WGPUDevice GetDevice() const { return m_Device; }
		WGPUQueue GetQueue() const { return m_Queue; }
		WGPUInstance GetInstance() const { return m_Instance; }
		WGPUSurface GetSurface() const { return m_Surface; }

	private:
		WGPUInstance m_Instance = nullptr;
		WGPUAdapter m_Adapter = nullptr;
		WGPUDevice m_Device = nullptr;
		WGPUQueue m_Queue = nullptr;
		WGPUSurface m_Surface = nullptr;
	};
}