//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "Component.h"
#include "SubMesh.h"

#include <vector>

namespace Rainbow {
	struct MeshComponent : public Component {
		void AddSubMesh(const SubMesh &mesh) {
			submesh.push_back(mesh);
		}
	private:
		std::vector<SubMesh> submesh;
	};
}