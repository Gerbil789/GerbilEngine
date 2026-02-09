#include "enginepch.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include "Engine/Graphics/Renderer/RenderGlobals.h"

namespace Engine
{
	Shader::Shader(const ShaderSpecification& specification, const std::string& source, const std::string& name)
	{
		m_Specification = specification;
		m_Name = name;
		LOG_INFO("Creating shader: {}", m_Name);

		wgpu::ShaderModule shaderModule = CreateShaderModule(source);

		auto vertexAttributeCount = specification.vertexAttributes.size();

		LOG_TRACE("Shader Vertex Attribute Count: {}", vertexAttributeCount);

		uint64_t offset = 0;
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
		vertexBufferLayout.arrayStride = offset;
		vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;

		wgpu::RenderPipelineDescriptor pipelineDesc;
		std::string pipelineLabel = m_Name + "ShaderPipeline";
		pipelineDesc.label = { pipelineLabel.c_str(), WGPU_STRLEN};

		pipelineDesc.vertex.bufferCount = 1;
		pipelineDesc.vertex.buffers = &vertexBufferLayout;
		pipelineDesc.vertex.module = shaderModule;
		pipelineDesc.vertex.entryPoint = { m_Specification.vsEntryPoint.c_str(), WGPU_STRLEN};
		pipelineDesc.vertex.constantCount = 0;
		pipelineDesc.vertex.constants = nullptr;

		pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
		pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined; // only for strip topologies
		pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
		pipelineDesc.primitive.cullMode = wgpu::CullMode::Back; //TODO: Configure in material?

		wgpu::BlendState blendState;
		blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
		blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
		blendState.color.operation = wgpu::BlendOperation::Add;
		blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
		blendState.alpha.dstFactor = wgpu::BlendFactor::One;
		blendState.alpha.operation = wgpu::BlendOperation::Add;

		wgpu::ColorTargetState colorTarget;
		colorTarget.format = wgpu::TextureFormat::RGBA8Unorm;
		colorTarget.blend = &blendState;
		colorTarget.writeMask = wgpu::ColorWriteMask::All;

		wgpu::FragmentState fragmentState;
		fragmentState.module = shaderModule;
		fragmentState.entryPoint = { m_Specification.fsEntryPoint.c_str(), WGPU_STRLEN };
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
			RenderGlobals::GetFrameLayout(),
			RenderGlobals::GetModelLayout(),
			m_MaterialBindGroupLayout
		};

		wgpu::PipelineLayoutDescriptor layoutDesc{};
		std::string label = m_Name + "ShaderPipelineLayout";
		layoutDesc.label = { label.c_str(), WGPU_STRLEN};
		layoutDesc.bindGroupLayoutCount = 3;
		layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayouts;
		pipelineDesc.layout = GraphicsContext::GetDevice().createPipelineLayout(layoutDesc);

		m_RenderPipeline = GraphicsContext::GetDevice().createRenderPipeline(pipelineDesc);

		shaderModule.release();
	}

	wgpu::ShaderModule Shader::CreateShaderModule(const std::string& source)
	{
		const char* shaderSource = source.c_str();

		wgpu::ShaderModuleDescriptor shaderDesc;
		std::string label = m_Name + "ShaderModule";
		shaderDesc.label = { label.c_str(), WGPU_STRLEN };

		wgpu::ShaderSourceWGSL shaderCodeDesc;
		shaderCodeDesc.chain.next = nullptr;
		shaderCodeDesc.chain.sType = wgpu::SType::ShaderSourceWGSL;
		shaderDesc.nextInChain = &shaderCodeDesc.chain;
		shaderCodeDesc.code = { shaderSource, WGPU_STRLEN };
		return GraphicsContext::GetDevice().createShaderModule(shaderDesc);
	}

	wgpu::BindGroupLayout Shader::CreateMaterialBindGroupLayout(const ShaderSpecification& specification)
	{
		auto materialBindings = GetMaterialBindings(specification);

		if(materialBindings.size() == 0)
		{
			return nullptr;
		}

		std::vector<wgpu::BindGroupLayoutEntry> layoutEntries;
		layoutEntries.reserve(materialBindings.size());

		for (const auto& binding : materialBindings)
		{
			wgpu::BindGroupLayoutEntry entry = wgpu::Default;
			entry.binding = binding.binding;
			entry.visibility = binding.visibility;

			switch (binding.type)
			{
			case BindingType::UniformBuffer:
				entry.buffer.type = wgpu::BufferBindingType::Uniform;
				entry.buffer.hasDynamicOffset = false;
				entry.buffer.minBindingSize = 0;
				m_MaterialUniformBufferSize = binding.size; //TODO: handle multiple uniform buffers?
				break;

			case BindingType::Texture2D:
				entry.texture.sampleType = binding.textureSample;
				entry.texture.viewDimension = wgpu::TextureViewDimension::_2D;
				entry.texture.multisampled = false;
				entry.visibility = wgpu::ShaderStage::Fragment;
				break;

			case BindingType::Sampler:
				entry.sampler.type = binding.samplerType;
				entry.visibility = wgpu::ShaderStage::Fragment;
				break;

			default:
				LOG_WARNING("Unsupported binding type in material bind group layout creation");
				continue;
			}

			layoutEntries.push_back(entry);
		}

		wgpu::BindGroupLayoutDescriptor desc{};
		std::string label = m_Name + "MaterialBindGroupLayout";
		desc.label = { label.c_str(), WGPU_STRLEN};
		desc.entryCount = static_cast<uint32_t>(layoutEntries.size());
		desc.entries = layoutEntries.data();

		return GraphicsContext::GetDevice().createBindGroupLayout(desc);
	}
	
}