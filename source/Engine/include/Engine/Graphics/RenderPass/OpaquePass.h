#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace engine
{
	class OpaquePass : public RenderPass
	{
	public:
		virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context) override;
	};
}