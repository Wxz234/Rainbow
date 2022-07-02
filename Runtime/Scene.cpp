//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Runtime/Scene.h"

#include <cassert>

namespace Rainbow {
	class Scene::Impl {
	public:
	};

	void CreateScene(Scene** ppScene) {
		assert(ppScene);
		Scene* pScene = new Scene;
		pScene->pimpl = new Scene::Impl;
		*ppScene = pScene;
	}
	void RemoveScene(Scene* pScene) {
		assert(pScene);
		delete pScene->pimpl;
		delete pScene;
	}
}