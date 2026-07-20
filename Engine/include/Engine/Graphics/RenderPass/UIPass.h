#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace Engine
{
	class ENGINE_API UIPass : public RenderPass
	{
	public:
		UIPass();
		virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context) override;
	};
}