#pragma once

#include "Engine/Core/Core.h"

namespace Engine 
{
	class UniformBuffer
	{
	public:
		UniformBuffer(uint32_t size, uint32_t binding) {};
		~UniformBuffer() = default;

		void SetData(const void* data, uint32_t size, uint32_t offset = 0);
		void Bind(uint32_t binding) const;

	};
}