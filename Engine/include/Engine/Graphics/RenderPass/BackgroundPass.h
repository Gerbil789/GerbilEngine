#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace Engine
{
	class ENGINE_API BackgroundPass : public RenderPass
	{
	public:
		virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context, const DrawList&) override;
	};
}