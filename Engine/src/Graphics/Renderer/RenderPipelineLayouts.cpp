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
		entries[0].buffer.hasDynamicOffset = true;
		entries[0].buffer.type = wgpu::BufferBindingType::Uniform;
		entries[0].buffer.minBindingSize = sizeof(ModelUniforms);

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.label = { "ModelBindGroupLayout", WGPU_STRLEN };
		bindGroupLayoutDesc.entryCount = entries.size();
		bindGroupLayoutDesc.entries = entries.data();
		s_ModelBindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);
	}

	void CreateEnvironmentBindGroupLayout()
	{
		std::array<wgpu::BindGroupLayoutEntry, 7> entries;

		// Environment uniforms
		entries[0].binding = 0;
		entries[0].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
		entries[0].buffer.type = wgpu::BufferBindingType::Uniform;
		entries[0].buffer.minBindingSize = sizeof(EnvironmentUniforms);

		// Environment sampler
		entries[1].binding = 1;
		entries[1].visibility = wgpu::ShaderStage::Fragment;
		entries[1].sampler.type = wgpu::SamplerBindingType::Filtering;

		// BRDF integration texture
		entries[2].binding = 2;
		entries[2].visibility = wgpu::ShaderStage::Fragment;
		entries[2].texture.sampleType = wgpu::TextureSampleType::Float;
		entries[2].texture.viewDimension = wgpu::TextureViewDimension::_2D;
		entries[2].texture.multisampled = false;

		// Irradiance texture
		entries[3].binding = 3;
		entries[3].visibility = wgpu::ShaderStage::Fragment;
		entries[3].texture.sampleType = wgpu::TextureSampleType::Float;
		entries[3].texture.viewDimension = wgpu::TextureViewDimension::Cube;
		entries[3].texture.multisampled = false;

		// Prefiltered environment texture
		entries[4].binding = 4;
		entries[4].visibility = wgpu::ShaderStage::Fragment;
		entries[4].texture.sampleType = wgpu::TextureSampleType::Float;
		entries[4].texture.viewDimension = wgpu::TextureViewDimension::Cube;
		entries[4].texture.multisampled = false;

		// Shadow map sampler
		entries[5].binding = 5;
		entries[5].visibility = wgpu::ShaderStage::Fragment;
		entries[5].sampler.type = wgpu::SamplerBindingType::Comparison;

		// Shadow map texture
		entries[6].binding = 6;
		entries[6].visibility = wgpu::ShaderStage::Fragment;
		entries[6].texture.sampleType = wgpu::TextureSampleType::Depth;
		entries[6].texture.viewDimension = wgpu::TextureViewDimension::_2DArray;
		entries[6].texture.multisampled = false;


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