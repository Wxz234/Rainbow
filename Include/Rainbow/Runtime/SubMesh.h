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
		std::vector<Vector3> normals;
		std::vector<Vector3> tangents;
		std::vector<uint16_t> indices;
		std::vector<Vector2> uv;
	};
}