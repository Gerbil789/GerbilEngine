#include "stb_image_write.h"
#include "Engine/Core/Log.h"
#include <webgpu/webgpu.hpp>
#include "Engine/Graphics/GraphicsContext.h"

#include <filesystem>
#include <string>

uint64_t m_bufferSize = 0;

bool saveTexture(const std::filesystem::path path, wgpu::Texture texture, int mipLevel) {
	using namespace wgpu;

	if (texture.getDimension() != TextureDimension::_2D) {
		throw std::runtime_error("Only 2D textures are supported by save_texture.h!");
	}
	uint32_t width = texture.getWidth() / (1 << mipLevel);
	uint32_t height = texture.getHeight() / (1 << mipLevel);
	uint32_t channels = 4; // TODO: infer from format
	uint32_t componentByteSize = 1; // TODO: infer from format

	uint32_t bytesPerRow = componentByteSize * channels * width;
	// Special case: WebGPU spec forbids texture-to-buffer copy with a
	// bytesPerRow lower than 256 so we first copy to a temporary texture.
	uint32_t paddedBytesPerRow = std::max(256u, bytesPerRow);

	m_bufferSize = paddedBytesPerRow * height;

	// Create a buffer to get pixels
	BufferDescriptor pixelBufferDesc = Default;
	pixelBufferDesc.mappedAtCreation = false;
	pixelBufferDesc.usage = BufferUsage::MapRead | BufferUsage::CopyDst;
	pixelBufferDesc.size = m_bufferSize;
	Buffer pixelBuffer = Engine::GraphicsContext::GetDevice().createBuffer(pixelBufferDesc);

	// Start encoding the commands
	CommandEncoder encoder = Engine::GraphicsContext::GetDevice().createCommandEncoder(Default);

	// Get pixels from texture to buffer
	wgpu::TexelCopyTextureInfo source = Default;
	source.texture = texture;
	source.mipLevel = mipLevel;

	wgpu::TexelCopyBufferInfo destination = Default;
	destination.buffer = pixelBuffer;
	destination.layout.bytesPerRow = paddedBytesPerRow;
	destination.layout.offset = 0;
	destination.layout.rowsPerImage = height;
	encoder.copyTextureToBuffer(source, destination, { width, height, 1 });

	// Issue commands
	CommandBuffer command = encoder.finish(Default);
	Engine::GraphicsContext::GetQueue().submit(command);


	struct MapContext
	{
		wgpu::Buffer buffer;
		const unsigned char* pixelData = nullptr;
	};

	auto ctx = new MapContext();
	ctx->buffer = pixelBuffer;

	wgpu::BufferMapCallbackInfo callbackInfo;
	callbackInfo.mode = wgpu::CallbackMode::AllowProcessEvents;
	callbackInfo.userdata1 = ctx;

	callbackInfo.callback = [](WGPUMapAsyncStatus status, WGPUStringView message, void* userdata1, void*) {
		auto* ctx = static_cast<MapContext*>(userdata1);
		if (status == wgpu::MapAsyncStatus::Success)
		{
			ctx->pixelData = (const unsigned char*)ctx->buffer.getConstMappedRange(0, m_bufferSize);
		}
		else
		{
			LOG_ERROR("Map failed: {}", message.data);
		}
	};


	wgpu::FutureWaitInfo waitInfo;
	waitInfo.future = pixelBuffer.mapAsync(wgpu::MapMode::Read, 0, m_bufferSize, callbackInfo);
	Engine::GraphicsContext::GetInstance().waitAny(1, &waitInfo, UINT64_MAX);

	int writeSuccess = stbi_write_png(path.string().c_str(), (int)width, (int)height, (int)channels, ctx->pixelData, paddedBytesPerRow);

	ctx->buffer.unmap();
	delete ctx;

	// Clean-up

	pixelBuffer.destroy();


	return writeSuccess;
}