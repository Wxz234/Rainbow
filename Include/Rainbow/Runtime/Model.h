//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "../Renderer/IRenderer.h"

#include "GameObject.h"

namespace Rainbow {
	struct Model : public GameObject {

	};

	void CreateGLTF(Device* pDevice, const char* file, Model** ppModel);
}