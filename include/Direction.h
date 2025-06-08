#pragma once

#include <cstdint>

namespace turing {
	enum class Direction : uint64_t {
		LEFT = 0,
		RIGHT = 1,
		UNCHANGED = 2
};
}