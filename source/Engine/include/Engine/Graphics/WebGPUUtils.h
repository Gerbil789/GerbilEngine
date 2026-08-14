#pragma once

#include "Engine/Core/Log.h"
#include "Engine/Utility/File.h"
#include "Engine/Graphics/GraphicsContext.h"
#include <webgpu/webgpu.hpp>

//TODO: rename this file to something better, its practicaly just toString() functions for webgpu enums

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
		wgslDesc.chain.next = nullptr;
		wgslDesc.chain.sType = wgpu::SType::ShaderSourceWGSL;
		wgslDesc.code = { source.c_str(), WGPU_STRLEN };

		wgpu::ShaderModuleDescriptor shaderDesc;
		shaderDesc.label = { label.c_str(), WGPU_STRLEN };
		shaderDesc.nextInChain = &wgslDesc.chain;

		return GraphicsContext::GetDevice().createShaderModule(shaderDesc);
	}



	inline static wgpu::VertexFormat StringToVertexFormat(const std::string& str)
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

		LOG_ERROR("Unknown vertex attribute format: {}", str);
		return wgpu::VertexFormat::Force32;
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

	static inline wgpu::ShaderStage StringToShaderStage(const std::string& stage)
	{
		uint32_t result = 0;

		if (stage.find("vertex") != std::string::npos)
			result |= static_cast<uint32_t>(wgpu::ShaderStage::Vertex);
		if (stage.find("fragment") != std::string::npos)
			result |= static_cast<uint32_t>(wgpu::ShaderStage::Fragment);
		if (stage.find("compute") != std::string::npos)
			result |= static_cast<uint32_t>(wgpu::ShaderStage::Compute);

		LOG_ERROR("Unknown shader stage: {}", stage);	
		return wgpu::ShaderStage(result);
	}

	inline static std::string VertexFormatToString(wgpu::VertexFormat format)
	{
		switch (format)
		{
		case wgpu::VertexFormat::Uint8x2:    return "Uint8x2";
		case wgpu::VertexFormat::Uint8x4:    return "Uint8x4";
		case wgpu::VertexFormat::Sint8x2:    return "Sint8x2";
		case wgpu::VertexFormat::Sint8x4:    return "Sint8x4";
		case wgpu::VertexFormat::Unorm8x2:   return "Unorm8x2";
		case wgpu::VertexFormat::Unorm8x4:   return "Unorm8x4";
		case wgpu::VertexFormat::Snorm8x2:   return "Snorm8x2";
		case wgpu::VertexFormat::Snorm8x4:   return "Snorm8x4";
		case wgpu::VertexFormat::Uint16x2:   return "Uint16x2";
		case wgpu::VertexFormat::Uint16x4:   return "Uint16x4";
		case wgpu::VertexFormat::Sint16x2:   return "Sint16x2";
		case wgpu::VertexFormat::Sint16x4:   return "Sint16x4";
		case wgpu::VertexFormat::Unorm16x2:  return "Unorm16x2";
		case wgpu::VertexFormat::Unorm16x4:  return "Unorm16x4";
		case wgpu::VertexFormat::Snorm16x2:  return "Snorm16x2";
		case wgpu::VertexFormat::Snorm16x4:  return "Snorm16x4";
		case wgpu::VertexFormat::Float16x2:  return "Float16x2";
		case wgpu::VertexFormat::Float16x4:  return "Float16x4";
		case wgpu::VertexFormat::Float32:    return "Float32";
		case wgpu::VertexFormat::Float32x2:  return "Float32x2";
		case wgpu::VertexFormat::Float32x3:  return "Float32x3";
		case wgpu::VertexFormat::Float32x4:  return "Float32x4";
		case wgpu::VertexFormat::Uint32:     return "Uint32";
		case wgpu::VertexFormat::Uint32x2:   return "Uint32x2";
		case wgpu::VertexFormat::Uint32x3:   return "Uint32x3";
		case wgpu::VertexFormat::Uint32x4:   return "Uint32x4";
		case wgpu::VertexFormat::Sint32:     return "Sint32";
		case wgpu::VertexFormat::Sint32x2:   return "Sint32x2";
		case wgpu::VertexFormat::Sint32x3:   return "Sint32x3";
		case wgpu::VertexFormat::Sint32x4:   return "Sint32x4";
		default: return "Unknown";
		}
	}

	inline static const std::string BackendTypeToString(wgpu::BackendType type)
	{
		switch (type)
		{
		case wgpu::BackendType::D3D11:    return "D3D11";
		case wgpu::BackendType::D3D12:    return "D3D12";
		case wgpu::BackendType::Metal:    return "Metal";
		case wgpu::BackendType::Vulkan:   return "Vulkan";
		case wgpu::BackendType::OpenGL:   return "OpenGL";
		case wgpu::BackendType::OpenGLES: return "OpenGLES";
		case wgpu::BackendType::Null:     return "Null";
		default:                          return "Unknown";
		}
	}

	inline static const std::string ErrorTypeToString(wgpu::ErrorType type)
	{
		switch (type)
		{
		case wgpu::ErrorType::NoError:      return "NoError";
		case wgpu::ErrorType::Validation:   return "Validation";
		case wgpu::ErrorType::OutOfMemory:  return "OutOfMemory";
		case wgpu::ErrorType::Internal:     return "Internal";
		case wgpu::ErrorType::Unknown:      return "Unknown";
		default:                            return "Unknown";
		}
	}


	inline std::string_view ToStringView(WGPUStringView sv)
	{
		if (!sv.data || sv.length == 0)
			return {};

		return std::string_view{ sv.data, sv.length };
	}

	inline std::string ToString(WGPUStringView sv)
	{
		if (!sv.data || sv.length == 0)
			return {};

		return std::string{ sv.data, sv.length };
	}

	inline std::string TextureFormatToString(wgpu::TextureFormat format)
	{
		switch (format)
		{
		case wgpu::TextureFormat::Undefined: return "Undefined";
		case wgpu::TextureFormat::R8Unorm: return "R8Unorm";
		case wgpu::TextureFormat::R8Snorm: return "R8Snorm";
		case wgpu::TextureFormat::R8Uint: return "R8Uint";
		case wgpu::TextureFormat::R8Sint: return "R8Sint";
		case wgpu::TextureFormat::R16Unorm: return "R16Unorm";
		case wgpu::TextureFormat::R16Snorm: return "R16Snorm";
		case wgpu::TextureFormat::R16Uint: return "R16Uint";
		case wgpu::TextureFormat::R16Sint: return "R16Sint";
		case wgpu::TextureFormat::R16Float: return "R16Float";
		case wgpu::TextureFormat::RG8Unorm: return "RG8Unorm";
		case wgpu::TextureFormat::RG8Snorm: return "RG8Snorm";
		case wgpu::TextureFormat::RG8Uint: return "RG8Uint";
		case wgpu::TextureFormat::RG8Sint: return "RG8Sint";
		case wgpu::TextureFormat::R32Float: return "R32Float";
		case wgpu::TextureFormat::R32Uint: return "R32Uint";
		case wgpu::TextureFormat::R32Sint: return "R32Sint";
		case wgpu::TextureFormat::RG16Unorm: return "RG16Unorm";
		case wgpu::TextureFormat::RG16Snorm: return "RG16Snorm";
		case wgpu::TextureFormat::RG16Uint: return "RG16Uint";
		case wgpu::TextureFormat::RG16Sint: return "RG16Sint";
		case wgpu::TextureFormat::RG16Float: return "RG16Float";
		case wgpu::TextureFormat::RGBA8Unorm: return "RGBA8Unorm";
		case wgpu::TextureFormat::RGBA8UnormSrgb: return "RGBA8UnormSrgb";
		case wgpu::TextureFormat::RGBA8Snorm: return "RGBA8Snorm";
		case wgpu::TextureFormat::RGBA8Uint: return "RGBA8Uint";
		case wgpu::TextureFormat::RGBA8Sint: return "RGBA8Sint";
		case wgpu::TextureFormat::BGRA8Unorm: return "BGRA8Unorm";
		case wgpu::TextureFormat::BGRA8UnormSrgb: return "BGRA8UnormSrgb";
		case wgpu::TextureFormat::RGB10A2Uint: return "RGB10A2Uint";
		case wgpu::TextureFormat::RGB10A2Unorm: return "RGB10A2Unorm";
		case wgpu::TextureFormat::RG11B10Ufloat: return "RG11B10Ufloat";
		case wgpu::TextureFormat::RGB9E5Ufloat: return "RGB9E5Ufloat";
		case wgpu::TextureFormat::RG32Float: return "RG32Float";
		case wgpu::TextureFormat::RG32Uint: return "RG32Uint";
		case wgpu::TextureFormat::RG32Sint: return "RG32Sint";
		case wgpu::TextureFormat::RGBA16Unorm: return "RGBA16Unorm";
		case wgpu::TextureFormat::RGBA16Snorm: return "RGBA16Snorm";
		case wgpu::TextureFormat::RGBA16Uint: return "RGBA16Uint";
		case wgpu::TextureFormat::RGBA16Sint: return "RGBA16Sint";
		case wgpu::TextureFormat::RGBA16Float: return "RGBA16Float";
		case wgpu::TextureFormat::RGBA32Float: return "RGBA32Float";
		case wgpu::TextureFormat::RGBA32Uint: return "RGBA32Uint";
		case wgpu::TextureFormat::RGBA32Sint: return "RGBA32Sint";
		case wgpu::TextureFormat::Stencil8: return "Stencil8";
		case wgpu::TextureFormat::Depth16Unorm: return "Depth16Unorm";
		case wgpu::TextureFormat::Depth24Plus: return "Depth24Plus";
		case wgpu::TextureFormat::Depth24PlusStencil8: return "Depth24PlusStencil8";
		case wgpu::TextureFormat::Depth32Float: return "Depth32Float";
		case wgpu::TextureFormat::Depth32FloatStencil8: return "Depth32FloatStencil8";
		case wgpu::TextureFormat::BC1RGBAUnorm: return "BC1RGBAUnorm";
		case wgpu::TextureFormat::BC1RGBAUnormSrgb: return "BC1RGBAUnormSrgb";
		case wgpu::TextureFormat::BC2RGBAUnorm: return "BC2RGBAUnorm";
		case wgpu::TextureFormat::BC2RGBAUnormSrgb: return "BC2RGBAUnormSrgb";
		case wgpu::TextureFormat::BC3RGBAUnorm: return "BC3RGBAUnorm";
		case wgpu::TextureFormat::BC3RGBAUnormSrgb: return "BC3RGBAUnormSrgb";
		case wgpu::TextureFormat::BC4RUnorm: return "BC4RUnorm";
		case wgpu::TextureFormat::BC4RSnorm: return "BC4RSnorm";
		case wgpu::TextureFormat::BC5RGUnorm: return "BC5RGUnorm";
		case wgpu::TextureFormat::BC5RGSnorm: return "BC5RGSnorm";
		case wgpu::TextureFormat::BC6HRGBUfloat: return "BC6HRGBUfloat";
		case wgpu::TextureFormat::BC6HRGBFloat: return "BC6HRGBFloat";
		case wgpu::TextureFormat::BC7RGBAUnorm: return "BC7RGBAUnorm";
		case wgpu::TextureFormat::BC7RGBAUnormSrgb: return "BC7RGBAUnormSrgb";
		case wgpu::TextureFormat::ETC2RGB8Unorm: return "ETC2RGB8Unorm";
		case wgpu::TextureFormat::ETC2RGB8UnormSrgb: return "ETC2RGB8UnormSrgb";
		case wgpu::TextureFormat::ETC2RGB8A1Unorm: return "ETC2RGB8A1Unorm";
		case wgpu::TextureFormat::ETC2RGB8A1UnormSrgb: return "ETC2RGB8A1UnormSrgb";
		case wgpu::TextureFormat::ETC2RGBA8Unorm: return "ETC2RGBA8Unorm";
		case wgpu::TextureFormat::ETC2RGBA8UnormSrgb: return "ETC2RGBA8UnormSrgb";
		case wgpu::TextureFormat::EACR11Unorm: return "EACR11Unorm";
		case wgpu::TextureFormat::EACR11Snorm: return "EACR11Snorm";
		case wgpu::TextureFormat::EACRG11Unorm: return "EACRG11Unorm";
		case wgpu::TextureFormat::EACRG11Snorm: return "EACRG11Snorm";
		case wgpu::TextureFormat::ASTC4x4Unorm: return "ASTC4x4Unorm";
		case wgpu::TextureFormat::ASTC4x4UnormSrgb: return "ASTC4x4UnormSrgb";
		case wgpu::TextureFormat::ASTC5x4Unorm: return "ASTC5x4Unorm";
		case wgpu::TextureFormat::ASTC5x4UnormSrgb: return "ASTC5x4UnormSrgb";
		case wgpu::TextureFormat::ASTC5x5Unorm: return "ASTC5x5Unorm";
		case wgpu::TextureFormat::ASTC5x5UnormSrgb: return "ASTC5x5UnormSrgb";
		case wgpu::TextureFormat::ASTC6x5Unorm: return "ASTC6x5Unorm";
		case wgpu::TextureFormat::ASTC6x5UnormSrgb: return "ASTC6x5UnormSrgb";
		case wgpu::TextureFormat::ASTC6x6Unorm: return "ASTC6x6Unorm";
		case wgpu::TextureFormat::ASTC6x6UnormSrgb: return "ASTC6x6UnormSrgb";
		case wgpu::TextureFormat::ASTC8x5Unorm: return "ASTC8x5Unorm";
		case wgpu::TextureFormat::ASTC8x5UnormSrgb: return "ASTC8x5UnormSrgb";
		case wgpu::TextureFormat::ASTC8x6Unorm: return "ASTC8x6Unorm";
		case wgpu::TextureFormat::ASTC8x6UnormSrgb: return "ASTC8x6UnormSrgb";
		case wgpu::TextureFormat::ASTC8x8Unorm: return "ASTC8x8Unorm";
		case wgpu::TextureFormat::ASTC8x8UnormSrgb: return "ASTC8x8UnormSrgb";
		case wgpu::TextureFormat::ASTC10x5Unorm: return "ASTC10x5Unorm";
		case wgpu::TextureFormat::ASTC10x5UnormSrgb: return "ASTC10x5UnormSrgb";
		case wgpu::TextureFormat::ASTC10x6Unorm: return "ASTC10x6Unorm";
		case wgpu::TextureFormat::ASTC10x6UnormSrgb: return "ASTC10x6UnormSrgb";
		case wgpu::TextureFormat::ASTC10x8Unorm: return "ASTC10x8Unorm";
		case wgpu::TextureFormat::ASTC10x8UnormSrgb: return "ASTC10x8UnormSrgb";
		case wgpu::TextureFormat::ASTC10x10Unorm: return "ASTC10x10Unorm";
		case wgpu::TextureFormat::ASTC10x10UnormSrgb: return "ASTC10x10UnormSrgb";
		case wgpu::TextureFormat::ASTC12x10Unorm: return "ASTC12x10Unorm";
		case wgpu::TextureFormat::ASTC12x10UnormSrgb: return "ASTC12x10UnormSrgb";
		case wgpu::TextureFormat::ASTC12x12Unorm: return "ASTC12x12Unorm";
		case wgpu::TextureFormat::ASTC12x12UnormSrgb: return "ASTC12x12UnormSrgb";
		case wgpu::TextureFormat::R8BG8Biplanar420Unorm: return "R8BG8Biplanar420Unorm";
		case wgpu::TextureFormat::R10X6BG10X6Biplanar420Unorm: return "R10X6BG10X6Biplanar420Unorm";
		case wgpu::TextureFormat::R8BG8A8Triplanar420Unorm: return "R8BG8A8Triplanar420Unorm";
		case wgpu::TextureFormat::R8BG8Biplanar422Unorm: return "R8BG8Biplanar422Unorm";
		case wgpu::TextureFormat::R8BG8Biplanar444Unorm: return "R8BG8Biplanar444Unorm";
		case wgpu::TextureFormat::R10X6BG10X6Biplanar422Unorm: return "R10X6BG10X6Biplanar422Unorm";
		case wgpu::TextureFormat::R10X6BG10X6Biplanar444Unorm: return "R10X6BG10X6Biplanar444Unorm";
		case wgpu::TextureFormat::OpaqueYCbCrAndroid: return "OpaqueYCbCrAndroid";
		case wgpu::TextureFormat::Force32: return "Force32";
		default: return "Unknown";
		}
	}
}