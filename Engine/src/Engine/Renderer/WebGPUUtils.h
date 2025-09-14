#pragma once

#include "Engine/Core/Log.h"
#include <webgpu/webgpu.hpp>

namespace Engine
{
	static inline wgpu::VertexFormat StringToVertexFormat(const std::string& str)
	{
		static const std::unordered_map<std::string, wgpu::VertexFormat> formatMap = {
				{ "Uint8x2",   wgpu::VertexFormat::Uint8x2 },
				{ "Uint8x4",   wgpu::VertexFormat::Uint8x4 },
				{ "Sint8x2",   wgpu::VertexFormat::Sint8x2 },
				{ "Sint8x4",   wgpu::VertexFormat::Sint8x4 },
				{ "Unorm8x2",  wgpu::VertexFormat::Unorm8x2 },
				{ "Unorm8x4",  wgpu::VertexFormat::Unorm8x4 },
				{ "Snorm8x2",  wgpu::VertexFormat::Snorm8x2 },
				{ "Snorm8x4",  wgpu::VertexFormat::Snorm8x4 },

				{ "Uint16x2",  wgpu::VertexFormat::Uint16x2 },
				{ "Uint16x4",  wgpu::VertexFormat::Uint16x4 },
				{ "Sint16x2",  wgpu::VertexFormat::Sint16x2 },
				{ "Sint16x4",  wgpu::VertexFormat::Sint16x4 },
				{ "Unorm16x2", wgpu::VertexFormat::Unorm16x2 },
				{ "Unorm16x4", wgpu::VertexFormat::Unorm16x4 },
				{ "Snorm16x2", wgpu::VertexFormat::Snorm16x2 },
				{ "Snorm16x4", wgpu::VertexFormat::Snorm16x4 },

				{ "Float16x2", wgpu::VertexFormat::Float16x2 },
				{ "Float16x4", wgpu::VertexFormat::Float16x4 },

				{ "Float32",   wgpu::VertexFormat::Float32 },
				{ "Float32x2", wgpu::VertexFormat::Float32x2 },
				{ "Float32x3", wgpu::VertexFormat::Float32x3 },
				{ "Float32x4", wgpu::VertexFormat::Float32x4 },

				{ "Uint32",    wgpu::VertexFormat::Uint32 },
				{ "Uint32x2",  wgpu::VertexFormat::Uint32x2 },
				{ "Uint32x3",  wgpu::VertexFormat::Uint32x3 },
				{ "Uint32x4",  wgpu::VertexFormat::Uint32x4 },

				{ "Sint32",    wgpu::VertexFormat::Sint32 },
				{ "Sint32x2",  wgpu::VertexFormat::Sint32x2 },
				{ "Sint32x3",  wgpu::VertexFormat::Sint32x3 },
				{ "Sint32x4",  wgpu::VertexFormat::Sint32x4 }
		};

		auto it = formatMap.find(str);
		if (it != formatMap.end())
			return it->second;

		ASSERT(false, "Unknown vertex attribute format: {}", str);
	}

	static inline uint32_t GetVertexFormatSize(wgpu::VertexFormat format)
	{
		switch (format)
		{
		case wgpu::VertexFormat::Uint8x2:
		case wgpu::VertexFormat::Sint8x2:
		case wgpu::VertexFormat::Unorm8x2:
		case wgpu::VertexFormat::Snorm8x2: return 2;

		case wgpu::VertexFormat::Uint8x4:
		case wgpu::VertexFormat::Sint8x4:
		case wgpu::VertexFormat::Unorm8x4:
		case wgpu::VertexFormat::Snorm8x4: return 4;

		case wgpu::VertexFormat::Uint16x2:
		case wgpu::VertexFormat::Sint16x2:
		case wgpu::VertexFormat::Unorm16x2:
		case wgpu::VertexFormat::Snorm16x2: return 4;

		case wgpu::VertexFormat::Uint16x4:
		case wgpu::VertexFormat::Sint16x4:
		case wgpu::VertexFormat::Unorm16x4:
		case wgpu::VertexFormat::Snorm16x4: return 8;

		case wgpu::VertexFormat::Float16x2: return 4;
		case wgpu::VertexFormat::Float16x4: return 8;

		case wgpu::VertexFormat::Float32:   return 4;
		case wgpu::VertexFormat::Float32x2: return 8;
		case wgpu::VertexFormat::Float32x3: return 12;
		case wgpu::VertexFormat::Float32x4: return 16;

		case wgpu::VertexFormat::Uint32:    return 4;
		case wgpu::VertexFormat::Uint32x2:  return 8;
		case wgpu::VertexFormat::Uint32x3:  return 12;
		case wgpu::VertexFormat::Uint32x4:  return 16;

		case wgpu::VertexFormat::Sint32:    return 4;
		case wgpu::VertexFormat::Sint32x2:  return 8;
		case wgpu::VertexFormat::Sint32x3:  return 12;
		case wgpu::VertexFormat::Sint32x4:  return 16;

			LOG_ERROR("Unknown vertex format size");
		default: return 0;
		}
	}

	static inline wgpu::ShaderStage StringToShaderStage(const std::string& stage)
	{
		uint32_t result = 0;

		if (stage.find("vertex") != std::string::npos)
			result |= static_cast<uint32_t>(wgpu::ShaderStage::Vertex);
		if (stage.find("fragment") != std::string::npos)
			result |= static_cast<uint32_t>(wgpu::ShaderStage::Fragment);
		if (stage.find("compute") != std::string::npos)
			result |= static_cast<uint32_t>(wgpu::ShaderStage::Compute);

		ASSERT(result != 0, "Unknown shader stage: {}", stage);
		return wgpu::ShaderStage(result);
	}
}