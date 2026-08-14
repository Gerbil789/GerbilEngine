#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace Engine
{
	class ENGINE_API BackgroundPass : public RenderPass
	{
	public:
		BackgroundPass();
		void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context) override;
	};
}