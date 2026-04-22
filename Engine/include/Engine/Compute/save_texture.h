#pragma once

#include <webgpu/webgpu-raii.hpp>
#include <filesystem>
#include "Engine/Core/API.h"

bool ENGINE_API saveTexture(const std::filesystem::path path, wgpu::Texture texture, int mipLevel);