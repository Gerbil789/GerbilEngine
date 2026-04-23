#pragma once

#include "Engine/Core/API.h"

namespace RenderDoc
{
	void ENGINE_API Initialize();
	void ENGINE_API StartFrameCapture();
	void ENGINE_API EndFrameCapture();
}