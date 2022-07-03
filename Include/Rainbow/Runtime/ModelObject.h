//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "GameObject.h"

#include <vector>

namespace Rainbow {
	struct ModelObject : public GameObject {

		virtual bool IsRenderable() { return true; }
		virtual Component* GetComponent(size_t i) const {
			return nullptr;
		}
		virtual size_t GetComponentSize() const {
			return 0;
		}

	private:
	};
}