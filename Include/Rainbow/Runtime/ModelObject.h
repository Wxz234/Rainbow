//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "GameObject.h"
#include "SubMesh.h"

#include <vector>

namespace Rainbow {
	struct ModelObject : public GameObject {
		ModelObject() {}
		ModelObject(const ModelObject&) = default;
		ModelObject& operator=(const ModelObject&) = default;
		ModelObject(ModelObject&&) = default;
		ModelObject& operator=(ModelObject&&) = default;

		virtual bool IsRenderable() { return true; }

		void AddSubMesh(const SubMesh &submesh) {
			submeshes.push_back(submesh);
		}
	private:
		std::vector<SubMesh> submeshes;
	};
}