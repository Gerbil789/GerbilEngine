#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace Engine
{
	class OpaquePass : public RenderPass
	{
	public:
		OpaquePass();
		virtual ~OpaquePass();

		virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context) override;
	};
}