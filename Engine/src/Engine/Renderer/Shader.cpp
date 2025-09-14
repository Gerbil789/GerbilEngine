#include "enginepch.h"
#include "Shader.h"
#include "Engine/Renderer/GraphicsContext.h"
#include "Engine/Renderer/WebGPUUtils.h"

#include "Engine/Renderer/Renderer.h"
namespace Engine
{
	Shader::Shader(const ShaderSpecification& specification, const std::string& source)
	{
		m_Specification = specification;
		LOG_INFO("Creating shader: {}", specification.name);

		auto device = GraphicsContext::GetDevice();
		auto queue = GraphicsContext::GetQueue();

		wgpu::ShaderModule shaderModule = CreateShaderModule(source);

		// Vertex Attributes
		auto vertexAttributeCount = specification.vertexAttributes.size();

		LOG_TRACE("Shader Vertex Attribute Count: {}", vertexAttributeCount);

		uint32_t offset = 0;
		std::vector<wgpu::VertexAttribute> vertexAttribs(vertexAttributeCount);
		for (size_t i = 0; i < vertexAttributeCount; i++)
		{
			const auto& attr = specification.vertexAttributes[i];

			vertexAttribs[i].format = attr.format;
			vertexAttribs[i].offset = offset;
			vertexAttribs[i].shaderLocation = attr.location;
			
			LOG_TRACE("Vertex Attribute ({}) - Location: {}, Offset: {}", attr.label, attr.location, vertexAttribs[i].offset);
			offset += GetVertexFormatSize(attr.format);
		}

		wgpu::VertexBufferLayout vertexBufferLayout;
		vertexBufferLayout.attributeCount = static_cast<uint32_t>(vertexAttributeCount);
		vertexBufferLayout.attributes = vertexAttribs.data();
		vertexBufferLayout.arrayStride = offset * sizeof(float);
		vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;

		wgpu::RenderPipelineDescriptor pipelineDesc;
		std::string pipelineLabel = specification.name + "ShaderPipeline";
		pipelineDesc.label = { pipelineLabel.c_str(), WGPU_STRLEN};

		pipelineDesc.vertex.bufferCount = 1;
		pipelineDesc.vertex.buffers = &vertexBufferLayout;
		pipelineDesc.vertex.module = shaderModule;
		pipelineDesc.vertex.entryPoint = { specification.vsEntry.c_str(), WGPU_STRLEN};
		pipelineDesc.vertex.constantCount = 0;
		pipelineDesc.vertex.constants = nullptr;

		pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
		pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined; // only for strip topologies
		pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
		pipelineDesc.primitive.cullMode = wgpu::CullMode::None; //TODO: Add culling later

		wgpu::BlendState blendState;
		blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
		blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
		blendState.color.operation = wgpu::BlendOperation::Add;
		blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
		blendState.alpha.dstFactor = wgpu::BlendFactor::One;
		blendState.alpha.operation = wgpu::BlendOperation::Add;

		wgpu::ColorTargetState colorTarget;
		colorTarget.format = WGPUTextureFormat_RGBA8Unorm;
		colorTarget.blend = &blendState;
		colorTarget.writeMask = wgpu::ColorWriteMask::All;

		wgpu::FragmentState fragmentState;
		fragmentState.module = shaderModule;
		fragmentState.entryPoint = { specification.fsEntry.c_str(), WGPU_STRLEN };
		fragmentState.constantCount = 0;
		fragmentState.constants = nullptr;
		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTarget;
		pipelineDesc.fragment = &fragmentState;

		wgpu::DepthStencilState depthStencilState {};
		depthStencilState.depthCompare = wgpu::CompareFunction::Less;
		depthStencilState.depthWriteEnabled = wgpu::OptionalBool::True;
		depthStencilState.format = wgpu::TextureFormat::Depth24Plus;
		depthStencilState.stencilReadMask = 0xFFFFFFFF;
		depthStencilState.stencilWriteMask = 0xFFFFFFFF;
		pipelineDesc.depthStencil = &depthStencilState;

		pipelineDesc.multisample.count = 1;		// no multisampling
		pipelineDesc.multisample.mask = ~0u;	// all samples enabled
		pipelineDesc.multisample.alphaToCoverageEnabled = false;

		m_MaterialBindGroupLayout = CreateMaterialBindGroupLayout(specification);

		wgpu::BindGroupLayout bindGroupLayouts[] = {
			Renderer::GetFrameBindGroupLayout(),
			Renderer::GetModelBindGroupLayout(),
			m_MaterialBindGroupLayout
		};

		wgpu::PipelineLayoutDescriptor layoutDesc{};
		std::string label = specification.name + "ShaderPipelineLayout";
		layoutDesc.label = { label.c_str(), WGPU_STRLEN};
		layoutDesc.bindGroupLayoutCount = 3;
		layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayouts;
		pipelineDesc.layout = device.createPipelineLayout(layoutDesc);

		m_RenderPipeline = device.createRenderPipeline(pipelineDesc);

		shaderModule.release();
	}

	wgpu::ShaderModule Shader::CreateShaderModule(const std::string& source)
	{
		const char* shaderSource = source.c_str();

		wgpu::ShaderModuleDescriptor shaderDesc;
		std::string label = m_Specification.name + "ShaderModule";
		shaderDesc.label = { label.c_str(), WGPU_STRLEN };
		wgpu::ShaderModuleWGSLDescriptor shaderCodeDesc;
		shaderCodeDesc.chain.next = nullptr;
		shaderCodeDesc.chain.sType = wgpu::SType::ShaderSourceWGSL;
		shaderDesc.nextInChain = &shaderCodeDesc.chain;
		shaderCodeDesc.code = { shaderSource, WGPU_STRLEN };
		return GraphicsContext::GetDevice().createShaderModule(shaderDesc);
	}

	wgpu::BindGroupLayout Shader::CreateMaterialBindGroupLayout(const ShaderSpecification& specification)
	{
		auto materialSpec = specification.GetBindGroup("material");

		if (!materialSpec)
		{
			LOG_ERROR("Shader does not define a bind group labeled 'material'");
			return nullptr;
		}

		std::vector<wgpu::BindGroupLayoutEntry> layoutEntries;
		layoutEntries.reserve(materialSpec->bindings.size());

		for (const auto& binding : materialSpec->bindings)
		{
			wgpu::BindGroupLayoutEntry entry = wgpu::Default;
			entry.binding = binding.binding;
			entry.visibility = wgpu::ShaderStage::Fragment;

			switch (binding.type)
			{
			case ShaderSpecification::Binding::Type::UniformBuffer:
			case ShaderSpecification::Binding::Type::StorageBuffer:
				entry.buffer.type = binding.bufferType;
				entry.buffer.hasDynamicOffset = false;
				entry.buffer.minBindingSize = 0;
				break;

			case ShaderSpecification::Binding::Type::Texture:
				entry.texture.sampleType = binding.textureType;
				entry.texture.viewDimension = wgpu::TextureViewDimension::_2D;
				entry.texture.multisampled = false;
				break;

			case ShaderSpecification::Binding::Type::Sampler:
				entry.sampler.type = binding.samplerType;
				break;
			}

			layoutEntries.push_back(entry);
		}

		wgpu::BindGroupLayoutDescriptor desc{};
		std::string label = m_Specification.name + "MaterialBindGroupLayout";
		desc.label = { label.c_str(), WGPU_STRLEN};
		desc.entryCount = static_cast<uint32_t>(layoutEntries.size());
		desc.entries = layoutEntries.data();

		return GraphicsContext::GetDevice().createBindGroupLayout(desc);
	}
	
}