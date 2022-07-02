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
	struct Scene {
		Device* pDevice;
	};

	void CreateScene(Device *pDevice, Scene**ppScene);
	void RemoveScene(Scene* pScene);
}