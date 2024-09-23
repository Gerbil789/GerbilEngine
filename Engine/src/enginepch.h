#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

#include "Engine/Core/Log.h"
#include "Engine/Debug/Instrumentor.h"

#ifdef ENGINE_PLATFORM_WINDOWS
#include <Windows.h>
#endif 