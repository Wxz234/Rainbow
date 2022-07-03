//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Runtime/Scene.h"
#include "../Include/Rainbow/Runtime/ModelObject.h"

#include "../ThirdParty/json/json.hpp"

#include <fstream>
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

	void Scene::LoadModelFromFile(const char* file) {
		std::ifstream i(file);
		nlohmann::json j;
		i >> j;
	}
}