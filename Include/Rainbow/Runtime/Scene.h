//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "../Renderer/IRenderer.h"
#include "GameObject.h"

#include <vector>

namespace Rainbow {
	struct Scene {

		void AddGameObject(GameObject* pGo) {
			mGameObject.push_back(pGo);
		}

		Device* pDevice;
		std::vector<GameObject*> mGameObject;
	};

	void CreateScene(Device *pDevice, Scene**ppScene);
	void RemoveScene(Scene* pScene);
}