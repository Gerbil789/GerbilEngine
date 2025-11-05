#pragma once

#include "Engine/Core/Log.h"
#include <webgpu/webgpu.hpp>

namespace Engine
{
	static inline wgpu::VertexFormat StringToVertexFormat(const std::string& str)
	{
		static const std::unordered_map<std::string, wgpu::VertexFormat> formatMap = {
			{ "vec1f", wgpu::VertexFormat::Float32 },
			{ "vec2f", wgpu::VertexFormat::Float32x2 },
			{ "vec3f", wgpu::VertexFormat::Float32x3 },
			{ "vec4f", wgpu::VertexFormat::Float32x4 },

			{ "vec2h", wgpu::VertexFormat::Float16x2 },
			{ "vec4h", wgpu::VertexFormat::Float16x4 },

			{ "vec1i", wgpu::VertexFormat::Sint32 },
			{ "vec2i", wgpu::VertexFormat::Sint32x2 },
			{ "vec3i", wgpu::VertexFormat::Sint32x3 },
			{ "vec4i", wgpu::VertexFormat::Sint32x4 },

			{ "vec1u", wgpu::VertexFormat::Uint32 },
			{ "vec2u", wgpu::VertexFormat::Uint32x2 },
			{ "vec3u", wgpu::VertexFormat::Uint32x3 },
			{ "vec4u", wgpu::VertexFormat::Uint32x4 },

			{ "vec2s", wgpu::VertexFormat::Sint16x2 },
			{ "vec4s", wgpu::VertexFormat::Sint16x4 },

			{ "vec2us", wgpu::VertexFormat::Uint16x2 },
			{ "vec4us", wgpu::VertexFormat::Uint16x4 },

			{ "vec2b", wgpu::VertexFormat::Sint8x2 },
			{ "vec4b", wgpu::VertexFormat::Sint8x4 },

			{ "vec2ub", wgpu::VertexFormat::Uint8x2 },
			{ "vec4ub", wgpu::VertexFormat::Uint8x4 },
		};

		auto it = formatMap.find(str);
		if (it != formatMap.end())
			return it->second;

		ASSERT(false, "Unknown vertex attribute format: {}", str);
		return wgpu::VertexFormat::Force32;
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