#pragma once

#include "Engine/Core/Log.h"
#include "Engine/Utility/File.h"
#include "Engine/Graphics/GraphicsContext.h"

namespace Engine
{
	inline static wgpu::ShaderModule LoadWGSLShader(const std::filesystem::path& path)
	{
		std::string source;
		if (!Engine::ReadFile(path, source))
		{
			throw std::runtime_error("Failed to load shader: " + path.string());
		}

		std::string label = path.filename().string();

		wgpu::ShaderSourceWGSL wgslDesc;
		wgslDesc.nextInChain = nullptr;
		wgslDesc.sType = wgpu::SType::ShaderSourceWGSL;
		wgslDesc.code = source.c_str();

		wgpu::ShaderModuleDescriptor shaderDesc;
		shaderDesc.label = label.c_str();
		shaderDesc.nextInChain = &wgslDesc;

		return GraphicsContext::GetDevice().CreateShaderModule(&shaderDesc);
	}

	inline static uint32_t GetVertexFormatSize(wgpu::VertexFormat format)
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

		default:
			LOG_ERROR("Unknown vertex format size");
			return 0;
		}
	}
}