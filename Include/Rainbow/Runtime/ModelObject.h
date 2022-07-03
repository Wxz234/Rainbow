//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "GameObject.h"
#include "MeshComponent.h"

#include <memory>

namespace Rainbow {
	struct ModelObject : public GameObject {
		ModelObject():mesh(new MeshComponent) {}

		ModelObject(const ModelObject& r) = delete;
		ModelObject& operator=(const ModelObject& r) = delete;
		ModelObject(ModelObject&& r) noexcept = default;
		ModelObject& operator=(ModelObject&& r) noexcept = default;

		virtual bool IsRenderable() { return true; }
		virtual Component* GetComponent(size_t i) const {
			if (i == 0) {
				return mesh.get();
			}
			return nullptr;
		}
		virtual size_t GetComponentSize() const { return 1; }

	private:
		std::unique_ptr<MeshComponent> mesh;
	};
}