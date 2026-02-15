#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace Engine
{
	class ENGINE_API LightPass : public RenderPass
	{
	public:
		LightPass();
		virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context, const DrawList& drawList) override;
	};
}