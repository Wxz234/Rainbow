//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "../Renderer/IRenderer.h"

namespace Rainbow {
	struct Scene {
		//void BindDevice(Device* pDevice);
		class Impl;
		Impl* pimpl;
	};

	//void CreateScene(Device* pDevice, Scene** ppScene);
	//void RemoveScene(Scene* pScene);
}