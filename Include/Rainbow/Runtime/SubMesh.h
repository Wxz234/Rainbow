      //
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "../Math/Math.h"

#include <vector>
#include <cstdint>

namespace Rainbow {
	struct SubMesh {
		std::vector<Vector3> vertices;
		std::vector<uint16_t> indices;
	};
}