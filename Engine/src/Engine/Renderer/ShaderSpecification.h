#pragma once

#include <vector>
#include <webgpu/webgpu.hpp>

namespace Engine
{
	enum class BindingType {Undefined, UniformBuffer, StorageBuffer, Sampler, Texture2D, Texture3D };
	enum class ShaderValueType { Bool, Int, UInt, Float, Vec2, Vec3, Vec4, Mat3, Mat4 };

	struct ShaderParameter
	{
		std::string name;
		ShaderValueType type;
		size_t offset = 0;
		size_t size = 0;
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

	struct VertexAttribute
	{
		uint32_t location;
		std::string label;
		wgpu::VertexFormat format;
	};

	struct ShaderSpecification
	{
		std::vector<VertexAttribute> vertexAttributes;
		std::vector<Binding> bindings;

		std::string vsEntryPoint = "vs_main";
		std::string fsEntryPoint = "fs_main";
	};

	// Helper functions to get bindings by group

	inline std::vector<Binding> GetBindingsForGroup(const ShaderSpecification& spec, uint32_t groupIndex)
	{
		std::vector<Binding> result;
		for (const auto& binding : spec.bindings)
		{
			if (binding.group == groupIndex)
				result.push_back(binding); // NOTE: copy, but ok for now | TODO: optimize later
		}
		return result;
	}

	inline std::vector<Binding> GetFrameBindings(const ShaderSpecification& spec)
	{
		return GetBindingsForGroup(spec, 0);
	}

	inline std::vector<Binding> GetModelBindings(const ShaderSpecification& spec)
	{
		return GetBindingsForGroup(spec, 1);
	}

	inline std::vector<Binding> GetMaterialBindings(const ShaderSpecification& spec)
	{
		return GetBindingsForGroup(spec, 2);
	}

	inline Binding GetBinding(const std::vector<Binding>& bindings, const std::string& name)
	{
		for (const auto& binding : bindings)
		{
			if (binding.name == name)
				return binding; // copy
		}

		throw std::runtime_error("Binding not found: " + name); //TODO: better error handling
	}
}