//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "../Math/Math.h"
#include "Component.h"
#include "SubMesh.h"

#include <vector>

namespace Rainbow {
	struct MeshComponent : public Component {
		
		std::vector<SubMesh> submesh;
	};
}