#pragma once

#include <iostream>
#include <filesystem>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <bitset>

#include <thread>
#include <atomic>
#include <future>
#include <mutex>

#include <string>
#include <sstream>
#include <iomanip>
#include <array>
#include <vector>
#include <queue>
#include <list>
#include <tuple>
#include <variant>
#include <unordered_map>
#include <unordered_set>

#define _USE_MATH_DEFINES
#include <cmath>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Tunti/Core/Log.h"
#include "Tunti/Core/Base.h"
#include "Tunti/Core/Platform.h"