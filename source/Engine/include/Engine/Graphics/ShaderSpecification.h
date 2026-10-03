#pragma once

#include <vector>
#include <variant>
#include <optional>
#include <string>
#include <glm/glm.hpp>
#include <webgpu/webgpu_cpp.h>

namespace engine
{
	struct ShaderParameter
	{
		std::string name;
		size_t offset = 0;
		size_t size = 0;
		std::optional<uint32_t> arraySize = std::nullopt;

		// editor data
		std::variant<float, int32_t, uint32_t, bool, glm::vec2, glm::vec3, glm::vec4, glm::ivec2, glm::mat3, glm::mat4> defaultValue;
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
		uint32_t group = ~0u;
		uint32_t binding = ~0u;
		std::string name;
		wgpu::ShaderStage visibility = wgpu::ShaderStage::None;
		std::variant<BufferBinding, TextureBinding, SamplerBinding> data;

		explicit operator bool() const { return group != ~0u && binding != ~0u; }
	};

	struct ShaderSpecification
	{
		std::string vsEntryPoint = "vs_main";
		std::string fsEntryPoint = "fs_main";

		std::vector<wgpu::VertexAttribute> vertexAttributes;
		std::vector<Binding> bindings;
	};
}