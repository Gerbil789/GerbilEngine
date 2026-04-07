#include "enginepch.h"
#include "Engine/Graphics/Texture.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/WebGPUUtils.h"
#include "Engine/Compute/save_texture.h"

namespace Engine
{
	namespace
	{
		static Texture2D* s_DefaultWhiteTexture = nullptr;
		static Texture2D* s_DefaultNormalTexture = nullptr;
		static Texture2D* s_DefaultAmbientTexture = nullptr;

		//TODO: make this better
		uint32_t getMaxMipLevelCount(const wgpu::Extent3D& textureSize)
		{
			return std::bit_width(std::max(textureSize.width, textureSize.height));
		}
	}




	void GenerateMipmaps(wgpu::Texture& texture)
	{
		std::vector<wgpu::TextureView> textureMipViews;
		std::vector<wgpu::Extent3D> textureMipSizes;

		wgpu::TextureFormat format = texture.getFormat();

		wgpu::Extent3D baseSize = { texture.getWidth(), texture.getHeight(), 1 };
		auto mipCount = getMaxMipLevelCount(baseSize);

		wgpu::TextureViewDescriptor textureViewDesc;
		textureViewDesc.nextInChain = nullptr;
		textureViewDesc.aspect = wgpu::TextureAspect::All;
		textureViewDesc.baseArrayLayer = 0;
		textureViewDesc.arrayLayerCount = 6;
		textureViewDesc.dimension = wgpu::TextureViewDimension::_2DArray;
		textureViewDesc.format = format;
		textureViewDesc.mipLevelCount = 1;


		textureMipSizes.resize(mipCount);
		textureMipViews.reserve(mipCount);

		textureMipSizes[0] = baseSize;

		for (uint32_t level = 0; level < mipCount; ++level)
		{
			std::string label = "MIP level #" + std::to_string(level);
			textureViewDesc.label = { label.c_str(), WGPU_STRLEN };
			textureViewDesc.baseMipLevel = level;
			textureMipViews.push_back(texture.createView(textureViewDesc));

			if (level > 0)
			{
				wgpu::Extent3D previousSize = textureMipSizes[level - 1];
				textureMipSizes[level] = { previousSize.width / 2, previousSize.height / 2, 6 };
			}
		}


		wgpu::ShaderModule computeShaderModule = LoadWGSLShader("Resources/Engine/shaders/compute/mipmap.wgsl");

		// Create bind group layout
		std::array<wgpu::BindGroupLayoutEntry, 2> bindings = { wgpu::Default, wgpu::Default };

		bindings[0].binding = 0;
		bindings[0].texture.sampleType = wgpu::TextureSampleType::Float;
		bindings[0].texture.viewDimension = wgpu::TextureViewDimension::_2DArray;
		bindings[0].visibility = wgpu::ShaderStage::Compute;

		bindings[1].binding = 1;
		bindings[1].storageTexture.access = wgpu::StorageTextureAccess::WriteOnly;
		bindings[1].storageTexture.format = format;
		bindings[1].storageTexture.viewDimension = wgpu::TextureViewDimension::_2DArray;
		bindings[1].visibility = wgpu::ShaderStage::Compute;

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.entryCount = bindings.size();
		bindGroupLayoutDesc.entries = bindings.data();
		wgpu::BindGroupLayout bindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);

		// Create compute pipeline layout
		wgpu::PipelineLayoutDescriptor pipelineLayoutDesc;
		pipelineLayoutDesc.bindGroupLayoutCount = 1;
		pipelineLayoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayout;
		wgpu::PipelineLayout pipelineLayout = GraphicsContext::GetDevice().createPipelineLayout(pipelineLayoutDesc);

		// Create compute pipeline;
		wgpu::ComputePipelineDescriptor computePipelineDesc = wgpu::Default;
		computePipelineDesc.compute.entryPoint = { "computeMipMap", WGPU_STRLEN };
		computePipelineDesc.compute.module = computeShaderModule;
		computePipelineDesc.layout = pipelineLayout;
		wgpu::ComputePipeline computeMipmapPipeline = GraphicsContext::GetDevice().createComputePipeline(computePipelineDesc);


		auto CreateBindGroup = [&](uint32_t nextLevel)
			{
				std::array<wgpu::BindGroupEntry, 2> entries = { wgpu::Default, wgpu::Default };

				entries[0].binding = 0;
				entries[0].textureView = textureMipViews[nextLevel - 1];

				entries[1].binding = 1;
				entries[1].textureView = textureMipViews[nextLevel];

				wgpu::BindGroupDescriptor bindGroupDesc;
				bindGroupDesc.layout = bindGroupLayout;
				bindGroupDesc.entryCount = (uint32_t)entries.size();
				bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
				wgpu::BindGroup bindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
				return bindGroup;
			};

		auto encoder = Engine::GraphicsContext::GetDevice().createCommandEncoder();

		wgpu::ComputePassDescriptor computePassDesc;
		computePassDesc.timestampWrites = nullptr;
		wgpu::ComputePassEncoder computePass = encoder.beginComputePass(computePassDesc);

		computePass.setPipeline(computeMipmapPipeline);


		for (uint32_t nextLevel = 1; nextLevel < mipCount; ++nextLevel)
		{
			computePass.setBindGroup(0, CreateBindGroup(nextLevel), 0, nullptr);

			uint32_t invocationCountX = textureMipSizes[nextLevel].width;
			uint32_t invocationCountY = textureMipSizes[nextLevel].height;
			uint32_t workgroupSizePerDim = 8;
			uint32_t workgroupCountX = (invocationCountX + workgroupSizePerDim - 1) / workgroupSizePerDim;
			uint32_t workgroupCountY = (invocationCountY + workgroupSizePerDim - 1) / workgroupSizePerDim;
			computePass.dispatchWorkgroups(workgroupCountX, workgroupCountY, 6);
		}

		computePass.end();

		wgpu::CommandBuffer commandBuffer = encoder.finish();
		Engine::GraphicsContext::GetQueue().submit(1, &commandBuffer);
	}

	void ImportanceSample(wgpu::Texture& texture)
	{
		std::vector<wgpu::TextureView> textureMipViews;
		std::vector<wgpu::Extent3D> textureMipSizes;
		wgpu::TextureView inputTextureView;

		wgpu::TextureFormat format = texture.getFormat();

		wgpu::Extent3D baseSize = { texture.getWidth(), texture.getHeight(), 1 };
		auto mipCount = getMaxMipLevelCount(baseSize);
	

		wgpu::TextureViewDescriptor textureViewDesc;
		textureViewDesc.nextInChain = nullptr;
		textureViewDesc.aspect = wgpu::TextureAspect::All;
		textureViewDesc.baseArrayLayer = 0;
		textureViewDesc.arrayLayerCount = 6;
		textureViewDesc.dimension = wgpu::TextureViewDimension::_2DArray;
		textureViewDesc.format = format;
		textureViewDesc.mipLevelCount = 1;


		textureMipSizes.resize(mipCount);
		textureMipViews.reserve(mipCount);

		textureMipSizes[0] = baseSize;

		for (uint32_t level = 0; level < mipCount; ++level)
		{
			std::string label = "MIP level #" + std::to_string(level);
			textureViewDesc.label = { label.c_str(), WGPU_STRLEN };
			textureViewDesc.baseMipLevel = level;
			textureMipViews.push_back(texture.createView(textureViewDesc));

			if (level > 0)
			{
				wgpu::Extent3D previousSize = textureMipSizes[level - 1];
				textureMipSizes[level] = { previousSize.width / 2, previousSize.height / 2, 6 };
			}
		}

		textureViewDesc.dimension = wgpu::TextureViewDimension::Cube;
		textureViewDesc.label = { "Input texture view", WGPU_STRLEN };
		textureViewDesc.baseMipLevel = 0;
		textureViewDesc.mipLevelCount = 1;
		inputTextureView = texture.createView(textureViewDesc);

		wgpu::ShaderModule computeShaderModule = LoadWGSLShader("Resources/Engine/shaders/compute/importanceSampling.wgsl");

		// Create bind group layout
		std::array<wgpu::BindGroupLayoutEntry, 3> bindings = { wgpu::Default, wgpu::Default, wgpu::Default };

		bindings[0].binding = 0;
		bindings[0].sampler.type = wgpu::SamplerBindingType::Filtering;
		bindings[0].visibility = wgpu::ShaderStage::Compute;

		bindings[1].binding = 1;
		bindings[1].texture.sampleType = wgpu::TextureSampleType::Float;
		bindings[1].texture.viewDimension = wgpu::TextureViewDimension::Cube;
		bindings[1].visibility = wgpu::ShaderStage::Compute;

		bindings[2].binding = 2;
		bindings[2].storageTexture.access = wgpu::StorageTextureAccess::WriteOnly;
		bindings[2].storageTexture.format = format;
		bindings[2].storageTexture.viewDimension = wgpu::TextureViewDimension::_2DArray;
		bindings[2].visibility = wgpu::ShaderStage::Compute;

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.entryCount = bindings.size();
		bindGroupLayoutDesc.entries = bindings.data();
		wgpu::BindGroupLayout bindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);

		// Create compute pipeline layout
		wgpu::PipelineLayoutDescriptor pipelineLayoutDesc;
		pipelineLayoutDesc.bindGroupLayoutCount = 1;
		pipelineLayoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayout;
		wgpu::PipelineLayout pipelineLayout = GraphicsContext::GetDevice().createPipelineLayout(pipelineLayoutDesc);

		// Create compute pipeline;
		wgpu::ComputePipelineDescriptor computePipelineDesc = wgpu::Default;
		computePipelineDesc.compute.entryPoint = { "computeMipMap", WGPU_STRLEN };
		computePipelineDesc.compute.module = computeShaderModule;
		computePipelineDesc.layout = pipelineLayout;
		wgpu::ComputePipeline computeMipmapPipeline = GraphicsContext::GetDevice().createComputePipeline(computePipelineDesc);




		wgpu::SamplerDescriptor samplerDesc{};
		samplerDesc.label = { "LinearEnvSampler", WGPU_STRLEN };
		samplerDesc.magFilter = wgpu::FilterMode::Linear;
		samplerDesc.minFilter = wgpu::FilterMode::Linear;
		samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
		samplerDesc.maxAnisotropy = 1;
		wgpu::Sampler linearSampler = GraphicsContext::GetDevice().createSampler(samplerDesc);


		auto CreateBindGroup = [&](uint32_t nextLevel)
			{
				std::array<wgpu::BindGroupEntry, 3> entries = { wgpu::Default, wgpu::Default, wgpu::Default };

				entries[0].binding = 0;
				entries[0].sampler = linearSampler;

				entries[1].binding = 1;
				entries[1].textureView = inputTextureView; // the original texture is always sampled from

				entries[2].binding = 2;
				entries[2].textureView = textureMipViews[nextLevel];

				wgpu::BindGroupDescriptor bindGroupDesc;
				bindGroupDesc.layout = bindGroupLayout;
				bindGroupDesc.entryCount = (uint32_t)entries.size();
				bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
				wgpu::BindGroup bindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);
				return bindGroup;
			};

		auto encoder = Engine::GraphicsContext::GetDevice().createCommandEncoder();

		wgpu::ComputePassDescriptor computePassDesc;
		computePassDesc.timestampWrites = nullptr;
		wgpu::ComputePassEncoder computePass = encoder.beginComputePass(computePassDesc);

		computePass.setPipeline(computeMipmapPipeline);


		for (uint32_t nextLevel = 1; nextLevel < mipCount; ++nextLevel)
		{
			computePass.setBindGroup(0, CreateBindGroup(nextLevel), 0, nullptr);

			uint32_t invocationCountX = textureMipSizes[nextLevel].width;
			uint32_t invocationCountY = textureMipSizes[nextLevel].height;
			uint32_t workgroupSizePerDim = 8;
			uint32_t workgroupCountX = (invocationCountX + workgroupSizePerDim - 1) / workgroupSizePerDim;
			uint32_t workgroupCountY = (invocationCountY + workgroupSizePerDim - 1) / workgroupSizePerDim;
			computePass.dispatchWorkgroups(workgroupCountX, workgroupCountY, 6);
		}

		computePass.end();

		wgpu::CommandBuffer commandBuffer = encoder.finish();
		Engine::GraphicsContext::GetQueue().submit(1, &commandBuffer);
	}





	Texture2D::Texture2D(const TextureSpecification& specification, const void* data)
	{
		m_Width = specification.width;
		m_Height = specification.height;
		m_TextureFormat = specification.format;
		uint32_t mipCount = 1;


		if (specification.generateMips)
		{
			mipCount = getMaxMipLevelCount({ m_Width , m_Height , 1 });
		}

		uint32_t bytesPerPixel = 4;
		if (m_TextureFormat == wgpu::TextureFormat::RGBA32Float)
		{
			bytesPerPixel = 16;
		}

		if(m_TextureFormat == wgpu::TextureFormat::RGBA16Float)
		{
			bytesPerPixel = 8;
		}

		wgpu::TextureDescriptor textureDesc;
		textureDesc.dimension = wgpu::TextureDimension::_2D;
		textureDesc.format = m_TextureFormat;
		textureDesc.mipLevelCount = mipCount;
		textureDesc.sampleCount = 1;
		textureDesc.size = { m_Width, m_Height, 1 };

		//TODO: write more elegantly
		wgpu::TextureUsage usage = specification.usage;
		if(specification.generateMips)
		{
			usage = specification.usage | wgpu::TextureUsage::StorageBinding;
		}

		textureDesc.usage = usage;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		m_Texture = GraphicsContext::GetDevice().createTexture(textureDesc);

		wgpu::TexelCopyTextureInfo dst;
		dst.texture = m_Texture;
		dst.mipLevel = 0;
		dst.origin = { 0, 0, 0 };
		dst.aspect = wgpu::TextureAspect::All;

		wgpu::TexelCopyBufferLayout layout;
		layout.offset = 0;
		layout.bytesPerRow = m_Width * bytesPerPixel;
		layout.rowsPerImage = m_Height;

		wgpu::Extent3D size = { m_Width, m_Height, 1 };

		GraphicsContext::GetQueue().writeTexture(dst, data, m_Width * m_Height * bytesPerPixel, layout, size);


		if (specification.generateMips)
		{
			GenerateMipmaps(m_Texture);
		}


		wgpu::TextureViewDescriptor viewDesc;
		viewDesc.label = { "Texture2DView", WGPU_STRLEN };
		viewDesc.format = m_TextureFormat;
		viewDesc.dimension = wgpu::TextureViewDimension::_2D;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;
		viewDesc.aspect = wgpu::TextureAspect::All;
		m_TextureView = m_Texture.createView(viewDesc);
	}

	Texture2D* Texture2D::GetDefault()
	{
		if (!s_DefaultWhiteTexture)
		{
			TextureSpecification spec;
			spec.width = 1;
			spec.height = 1;
			spec.format = wgpu::TextureFormat::RGBA8Unorm;
			uint32_t whitePixel = 0xFFFFFFFF;
			s_DefaultWhiteTexture = new Texture2D(spec, &whitePixel);
		}

		return s_DefaultWhiteTexture;
	}

	Texture2D* Texture2D::GetDefaultNormal()
	{
		if (!s_DefaultNormalTexture)
		{
			TextureSpecification spec;
			spec.width = 1;
			spec.height = 1;
			spec.format = wgpu::TextureFormat::RGBA8Unorm;
			constexpr uint8_t normalPixel[4] = { 128, 128, 255, 255 }; // Blue-ish

			s_DefaultNormalTexture = new Texture2D(spec, &normalPixel);
		}

		return s_DefaultNormalTexture;
	}

	Texture2D* Texture2D::GetDefaultAmbient()
	{
		if (!s_DefaultAmbientTexture)
		{
			TextureSpecification spec;
			spec.width = 1;
			spec.height = 1;
			spec.format = wgpu::TextureFormat::RGBA8Unorm;
			constexpr uint8_t ambientPixel[4] = { 10, 10, 10, 255 };
			s_DefaultAmbientTexture = new Texture2D(spec, &ambientPixel);
		}
		return s_DefaultAmbientTexture;

	}

	SubTexture2D::SubTexture2D(Texture2D* texture, const glm::vec2& min, const glm::vec2& max) : m_Texture(texture), m_UVMin(min), m_UVMax(max) {}

	SubTexture2D* SubTexture2D::CreateFromGrid(Texture2D* texture, const glm::ivec2& cellCoords, const glm::ivec2& cellSize, const glm::ivec2& spriteSize)
	{
		glm::vec2 texSize = { static_cast<float>(texture->GetWidth()), static_cast<float>(texture->GetHeight()) };

		glm::vec2 min
		{
				(cellCoords.x * cellSize.x) / texSize.x,
				(cellCoords.y * cellSize.y) / texSize.y
		};

		glm::vec2 max
		{
				((cellCoords.x + spriteSize.x) * cellSize.x) / texSize.x,
				((cellCoords.y + spriteSize.y) * cellSize.y) / texSize.y
		};

		return new SubTexture2D(texture, min, max);
	}







	CubeMapTexture::CubeMapTexture(const TextureSpecification& specification, const void* data)
	{
		m_Width = specification.width;
		m_Height = specification.height;
		m_TextureFormat = specification.format;

		wgpu::TextureDescriptor textureDesc;
		textureDesc.dimension = wgpu::TextureDimension::_2D;
		textureDesc.format = m_TextureFormat;
		textureDesc.mipLevelCount = 1;
		textureDesc.sampleCount = 1;
		textureDesc.size = { m_Width, m_Height, 1 };
		textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
		textureDesc.viewFormatCount = 0;
		textureDesc.viewFormats = nullptr;
		wgpu::Texture inputTexture = GraphicsContext::GetDevice().createTexture(textureDesc);

		uint32_t bytesPerPixel = 4;
		if (m_TextureFormat == wgpu::TextureFormat::RGBA16Float)
		{
			bytesPerPixel = 8;
		}
		if(m_TextureFormat == wgpu::TextureFormat::RGBA32Float)
		{
			bytesPerPixel = 16;
		}

		wgpu::TexelCopyTextureInfo dst;
		dst.texture = inputTexture;
		dst.mipLevel = 0;
		dst.origin = { 0, 0, 0 };
		dst.aspect = wgpu::TextureAspect::All;

		wgpu::TexelCopyBufferLayout layout;
		layout.offset = 0;
		layout.bytesPerRow = m_Width * bytesPerPixel;
		layout.rowsPerImage = m_Height;

		wgpu::Extent3D size = { m_Width, m_Height, 1 };

		GraphicsContext::GetQueue().writeTexture(dst, data, m_Width * m_Height * bytesPerPixel, layout, size);


		uint32_t faceSize = m_Height / 2;
		uint32_t mipCount = getMaxMipLevelCount({ faceSize, faceSize, 1 });

		textureDesc.size = { faceSize, faceSize, 6 };
		textureDesc.mipLevelCount = mipCount;
		textureDesc.usage = wgpu::TextureUsage::StorageBinding | wgpu::TextureUsage::CopySrc | wgpu::TextureUsage::TextureBinding;
		m_Texture = GraphicsContext::GetDevice().createTexture(textureDesc);

		std::vector<wgpu::TextureView> m_cubemapTextureMips;

		wgpu::TextureViewDescriptor viewDesc;
		viewDesc.format = m_TextureFormat;
		viewDesc.dimension = wgpu::TextureViewDimension::_2DArray;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 6;
		viewDesc.aspect = wgpu::TextureAspect::All;
		m_cubemapTextureMips.resize(mipCount, nullptr);
		for (uint32_t level = 0; level < mipCount; ++level)
		{
			viewDesc.baseMipLevel = level;
			m_cubemapTextureMips[level] = m_Texture.createView(viewDesc);
		}

		viewDesc.arrayLayerCount = 1;
		viewDesc.baseMipLevel = 0;
		viewDesc.dimension = wgpu::TextureViewDimension::_2D;
		wgpu::TextureView inputTextureView = inputTexture.createView(viewDesc);


		viewDesc.dimension = wgpu::TextureViewDimension::Cube;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 6;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = mipCount;
		m_TextureView = m_Texture.createView(viewDesc);

		wgpu::ShaderModule computeShaderModule = LoadWGSLShader("Resources/Engine/shaders/compute/cubemap.wgsl");

		// Create bind group layout
		std::vector<wgpu::BindGroupLayoutEntry> bindings(3, wgpu::Default);

		bindings[0].binding = 0;
		bindings[0].sampler.type = wgpu::SamplerBindingType::Filtering;
		bindings[0].visibility = wgpu::ShaderStage::Compute;

		bindings[1].binding = 1;
		bindings[1].texture.sampleType = wgpu::TextureSampleType::Float;
		bindings[1].texture.viewDimension = wgpu::TextureViewDimension::_2D;
		bindings[1].visibility = wgpu::ShaderStage::Compute;

		bindings[2].binding = 2;
		bindings[2].storageTexture.access = wgpu::StorageTextureAccess::WriteOnly;
		bindings[2].storageTexture.format = m_TextureFormat;
		bindings[2].storageTexture.viewDimension = wgpu::TextureViewDimension::_2DArray;
		bindings[2].visibility = wgpu::ShaderStage::Compute;

		wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
		bindGroupLayoutDesc.entryCount = bindings.size();
		bindGroupLayoutDesc.entries = bindings.data();
		wgpu::BindGroupLayout bindGroupLayout = GraphicsContext::GetDevice().createBindGroupLayout(bindGroupLayoutDesc);

		// Create compute pipeline layout
		wgpu::PipelineLayoutDescriptor pipelineLayoutDesc;
		pipelineLayoutDesc.bindGroupLayoutCount = 1;
		pipelineLayoutDesc.bindGroupLayouts = (WGPUBindGroupLayout*)&bindGroupLayout;
		wgpu::PipelineLayout pipelineLayout = GraphicsContext::GetDevice().createPipelineLayout(pipelineLayoutDesc);

		// Create compute pipeline;
		wgpu::ComputePipelineDescriptor computePipelineDesc = wgpu::Default;
		computePipelineDesc.compute.entryPoint = { "equirectToCubemap", WGPU_STRLEN };
		computePipelineDesc.compute.module = computeShaderModule;
		computePipelineDesc.layout = pipelineLayout;
		wgpu::ComputePipeline computePipeline = GraphicsContext::GetDevice().createComputePipeline(computePipelineDesc);


		std::vector<wgpu::BindGroup> m_bindGroups;

		m_bindGroups.resize(mipCount, nullptr);

		std::vector<wgpu::BindGroupEntry> entries(3, wgpu::Default);

		entries[0].binding = 0;
		entries[0].sampler = GraphicsContext::GetDevice().createSampler();

		entries[1].binding = 1;
		entries[1].textureView = inputTextureView;

		entries[2].binding = 2;
		entries[2].textureView = m_cubemapTextureMips[0];

		wgpu::BindGroupDescriptor bindGroupDesc;
		bindGroupDesc.layout = bindGroupLayout;
		bindGroupDesc.entryCount = (uint32_t)entries.size();
		bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
		wgpu::BindGroup bindGroup = GraphicsContext::GetDevice().createBindGroup(bindGroupDesc);

		auto encoder = Engine::GraphicsContext::GetDevice().createCommandEncoder();

		wgpu::ComputePassDescriptor computePassDesc;
		computePassDesc.timestampWrites = nullptr;
		wgpu::ComputePassEncoder computePass = encoder.beginComputePass(computePassDesc);

		computePass.setPipeline(computePipeline);


		computePass.setBindGroup(0, bindGroup, 0, nullptr);

		uint32_t invocationCountX = faceSize;
		uint32_t invocationCountY = faceSize;
		uint32_t workgroupSizePerDim = 4;

		uint32_t workgroupCountX = (invocationCountX + workgroupSizePerDim - 1) / workgroupSizePerDim;
		uint32_t workgroupCountY = (invocationCountY + workgroupSizePerDim - 1) / workgroupSizePerDim;
		computePass.dispatchWorkgroups(workgroupCountX, workgroupCountY, 6);
		computePass.end();

		wgpu::CommandBuffer commandBuffer = encoder.finish();
		Engine::GraphicsContext::GetQueue().submit(1, &commandBuffer);

		ImportanceSample(m_Texture);

		//GenerateMipmaps(m_Texture);
	}
}