#include "enginepch.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Renderer/RenderUniforms.h"

namespace Engine
{
	namespace 
	{
		static wgpu::BindGroupLayout s_ViewBindGroupLayout = nullptr;
		static wgpu::BindGroupLayout s_ModelBindGroupLayout = nullptr;
		static wgpu::BindGroupLayout s_EnvironmentBindGroupLayout = nullptr;
	}

	void CreateViewBindGroupLayout()
	{
		std::array<wgpu::BindGroupLayoutEntry, 1> entries;

		entries[0].binding = 0;
		entries[0].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
		entries[0].buffer.type = wgpu::BufferBindingType::Uniform;
		entries[0].buffer.minBindingSize = sizeof(ViewUniforms);

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.label = { "ViewBindGroupLayout", WGPU_STRLEN };
		bindGroupLayoutDesc.entryCount = entries.size();
		bindGroupLayoutDesc.entries = entries.data();

		s_ViewBindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);
	}

	void CreateModelBindGroupLayout()
	{
		std::array<wgpu::BindGroupLayoutEntry, 1> entries;

		entries[0].binding = 0;
		entries[0].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
		entries[0].buffer.hasDynamicOffset = false;
		entries[0].buffer.type = wgpu::BufferBindingType::ReadOnlyStorage;
		entries[0].buffer.minBindingSize = sizeof(glm::mat4);

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.label = { "ModelBindGroupLayout", WGPU_STRLEN };
		bindGroupLayoutDesc.entryCount = entries.size();
		bindGroupLayoutDesc.entries = entries.data();
		s_ModelBindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);
	}

	void CreateEnvironmentBindGroupLayout()
	{
		std::array<wgpu::BindGroupLayoutEntry, 8> entries;

		// 0 - EnvironmentSampler
		{
			entries[0].binding = 0;
			entries[0].visibility = wgpu::ShaderStage::Fragment;
			entries[0].sampler.type = wgpu::SamplerBindingType::Filtering;
		}

		// 1 - EnvironmentMap
		{
			entries[1].binding = 1;
			entries[1].visibility = wgpu::ShaderStage::Fragment;
			entries[1].texture.sampleType = wgpu::TextureSampleType::Float;
			entries[1].texture.viewDimension = wgpu::TextureViewDimension::Cube;
			entries[1].texture.multisampled = false;
		}

		// 2 - IrradianceMap
		{
			entries[2].binding = 2;
			entries[2].visibility = wgpu::ShaderStage::Fragment;
			entries[2].texture.sampleType = wgpu::TextureSampleType::Float;
			entries[2].texture.viewDimension = wgpu::TextureViewDimension::Cube;
			entries[2].texture.multisampled = false;
		}

		// 3 - PrefilteredSpecularMap
		{
			entries[3].binding = 3;
			entries[3].visibility = wgpu::ShaderStage::Fragment;
			entries[3].texture.sampleType = wgpu::TextureSampleType::Float;
			entries[3].texture.viewDimension = wgpu::TextureViewDimension::Cube;
			entries[3].texture.multisampled = false;
		}

		// 4 - BRDFIntMap
		{
			entries[4].binding = 4;
			entries[4].visibility = wgpu::ShaderStage::Fragment;
			entries[4].texture.sampleType = wgpu::TextureSampleType::Float;
			entries[4].texture.viewDimension = wgpu::TextureViewDimension::_2D;
			entries[4].texture.multisampled = false;
		}

		// 5 - ShadowUniforms
		{
			entries[5].binding = 5;
			entries[5].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
			entries[5].buffer.type = wgpu::BufferBindingType::Uniform;
			entries[5].buffer.minBindingSize = sizeof(EnvironmentUniforms);
		}

		// 6 -ShadowSampler
		{
			entries[6].binding = 6;
			entries[6].visibility = wgpu::ShaderStage::Fragment;
			entries[6].sampler.type = wgpu::SamplerBindingType::Comparison;
		}

		// 7 - ShadowMap
		{
			entries[7].binding = 7;
			entries[7].visibility = wgpu::ShaderStage::Fragment;
			entries[7].texture.sampleType = wgpu::TextureSampleType::Depth;
			entries[7].texture.viewDimension = wgpu::TextureViewDimension::_2DArray;
			entries[7].texture.multisampled = false;
		}

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.label = { "EnvironmentBindGroupLayout", WGPU_STRLEN };
		bindGroupLayoutDesc.entryCount = entries.size();
		bindGroupLayoutDesc.entries = entries.data();

		s_EnvironmentBindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);
	}

	void RenderPipelineLayouts::Initialize()
	{
		CreateViewBindGroupLayout();
		CreateModelBindGroupLayout();
		CreateEnvironmentBindGroupLayout();
	}

	wgpu::BindGroupLayout RenderPipelineLayouts::GetViewLayout()
	{
		return s_ViewBindGroupLayout;
	}

	wgpu::BindGroupLayout RenderPipelineLayouts::GetModelLayout()
	{
		return s_ModelBindGroupLayout;
	}

	wgpu::BindGroupLayout RenderPipelineLayouts::GetEnvironmentLayout()
	{
		return s_EnvironmentBindGroupLayout;
	}
}