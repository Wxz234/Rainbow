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

	private:
		
	};
}