#pragma once

#include <vector>
#include <variant>
#include <optional>
#include <glm/glm.hpp>
#include <webgpu/webgpu.hpp>

namespace Engine
{
	enum class BindingType {Undefined, UniformBuffer, StorageBuffer, Sampler, Texture2D, Texture3D, TextureCube };
	enum class ShaderValueType { Bool, Int, UInt, Float, Vec2, Vec3, Vec4, Vec2i, Mat3, Mat4, ArrayMat4, ArrayFloat };

	struct ShaderParameter
	{
		std::string name;
		ShaderValueType type;
		size_t offset = 0;
		size_t size = 0;

		std::variant<float, glm::vec2, glm::vec3, glm::vec4, glm::ivec2> defaultValue;
		bool isColor = false;

		std::optional<float> min;
		std::optional<float> max;
	};

	struct Binding
	{
		uint32_t group;
		uint32_t binding;
		std::string name;
		BindingType type = BindingType::Undefined;
		wgpu::ShaderStage visibility = wgpu::ShaderStage::None;

		// For Uniform / Storage Buffers
		size_t size;
		std::vector<ShaderParameter> parameters;

		// For Textures
		wgpu::TextureSampleType textureSample = wgpu::TextureSampleType::Float;

		// For Samplers
		wgpu::SamplerBindingType samplerType = wgpu::SamplerBindingType::Filtering;
	};

	struct ShaderSpecification
	{
		std::vector<wgpu::VertexAttribute> vertexAttributes;
		std::vector<Binding> bindings;

		std::string vsEntryPoint = "vs_main";
		std::string fsEntryPoint = "fs_main";
	};
}