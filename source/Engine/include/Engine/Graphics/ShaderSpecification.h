#pragma once

#include <vector>
#include <variant>
#include <optional>
#include <string>
#include <glm/glm.hpp>
#include <webgpu/webgpu_cpp.h>

namespace Engine
{
	enum class ShaderValueType { Bool, Int, UInt, Float, Vec2, Vec3, Vec4, Vec2i, Mat3, Mat4 };

	struct ShaderParameter
	{
		std::string name;
		ShaderValueType type;
		size_t offset = 0;
		size_t size = 0;
		std::optional<uint32_t> arraySize = std::nullopt;

		// editor data
		std::variant<float, glm::vec2, glm::vec3, glm::vec4, glm::ivec2> defaultValue;
		std::optional<float> min;
		std::optional<float> max;
		bool isColor = false;
	};

	struct BufferBinding
	{
		wgpu::BufferBindingType type = wgpu::BufferBindingType::Undefined;  // Uniform, Storage, ReadOnlyStorage
		std::optional<uint32_t> arraySize = std::nullopt;
		size_t size = 0;
		std::vector<ShaderParameter> parameters;
	};

	struct TextureBinding
	{
		wgpu::TextureViewDimension viewDimension = wgpu::TextureViewDimension::Undefined;
		wgpu::TextureSampleType sampleType = wgpu::TextureSampleType::Undefined;
		bool isMultisampled = false;
	};

	struct SamplerBinding
	{
		wgpu::SamplerBindingType type = wgpu::SamplerBindingType::Undefined;
	};

	struct Binding
	{
		uint32_t group;
		uint32_t binding;
		std::string name;
		wgpu::ShaderStage visibility = wgpu::ShaderStage::None;	// Vertex, Fragment, Compute
		std::variant<BufferBinding, TextureBinding, SamplerBinding> data;
	};

	struct ShaderSpecification
	{
		std::string vsEntryPoint = "vs_main";
		std::string fsEntryPoint = "fs_main";

		std::vector<wgpu::VertexAttribute> vertexAttributes;
		std::vector<Binding> bindings;
	};
}