//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "GameObject.h"

namespace Rainbow {
	struct CameraObject : public GameObject {
		virtual bool IsRenderable() { return false; }

	private:

	};
}