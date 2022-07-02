//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Runtime/Scene.h"	

#include <cassert>

namespace Rainbow {
	void CreateScene(Device* pDevice, Scene** ppScene) {
		assert(pDevice);
		assert(ppScene);
		Scene* pScene = new Scene;
		pScene->pDevice = pDevice;
		*ppScene = pScene;
	}
	void RemoveScene(Scene* pScene) {
		assert(pScene);
		delete pScene;
	}
}