#pragma once

#include <vector>
#include <variant>
#include <optional>
#include <glm/glm.hpp>
#include <webgpu/webgpu.hpp>

namespace Engine
{
	enum class BindingType {Uniform, Storage, Sampler, Texture2D, TextureCube };
	enum class StorageAccess { None, Read, ReadWrite };
	enum class ShaderValueType { Bool, Int, UInt, Float, Vec2, Vec3, Vec4, Vec2i, Mat3, Mat4, ArrayMat4, ArrayFloat };

	struct ShaderParameter
	{
		std::string name;
		ShaderValueType type;
		std::variant<float, glm::vec2, glm::vec3, glm::vec4, glm::ivec2> defaultValue;
		size_t offset = 0;
		size_t size = 0;

		std::optional<float> min;
		std::optional<float> max;

		bool isColor = false;
	};

	struct Binding
	{
		uint32_t group;
		uint32_t binding;
		std::string name;
		BindingType type;
		wgpu::ShaderStage visibility = wgpu::ShaderStage::None;

		// For Uniform / Storage Buffers
		size_t size;
		std::vector<ShaderParameter> parameters;

		// For Textures
		wgpu::TextureSampleType textureSample = wgpu::TextureSampleType::Float;
	};

	struct ShaderSpecification
	{
		std::vector<wgpu::VertexAttribute> vertexAttributes;
		std::vector<Binding> bindings;

		std::string vsEntryPoint = "vs_main";
		std::string fsEntryPoint = "fs_main";
	};
}