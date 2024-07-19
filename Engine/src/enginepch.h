#pragma once

#ifdef ENGINE_PLATFORM_WINDOWS
#include <Windows.h>
#endif 

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

#include "Engine/Core/Log.h"

// debug
#include "Engine/Debug/Instrumentor.h"