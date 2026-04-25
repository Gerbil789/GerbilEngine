#pragma once

#include <webgpu/webgpu-raii.hpp>
#include <filesystem>

bool ENGINE_API saveTexture(const std::filesystem::path path, wgpu::Texture texture, int mipLevel);