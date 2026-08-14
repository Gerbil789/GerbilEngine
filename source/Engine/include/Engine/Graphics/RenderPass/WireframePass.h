#pragma once

#include "Engine/Graphics/RenderPass/RenderPass.h"

namespace Engine
{
	class ENGINE_API WireframePass : public RenderPass
	{
	public:
		WireframePass();
		void SetColor(const glm::vec4& color);
		virtual void Execute(wgpu::CommandEncoder& encoder, const RenderContext& context) override;

	};
}