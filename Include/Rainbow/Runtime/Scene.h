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
			if (pGo->IsRenderable()) {
				mRenderableObject.push_back(pGo);
			}
		}

		void LoadModelFromFile(const char* file);

		Device* pDevice = nullptr;
		std::vector<GameObject*> mGameObject;
		std::vector<GameObject*> mRenderableObject;
		std::vector<Component*> mComponent;
	};

	void CreateScene(Device *pDevice, Scene**ppScene);
	void RemoveScene(Scene* pScene);
}