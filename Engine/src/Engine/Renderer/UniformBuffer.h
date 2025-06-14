#pragma once

#include "Engine/Core/Core.h"

namespace Engine 
{
	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		virtual void Bind(uint32_t binding) const = 0;

		static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);


	};

}