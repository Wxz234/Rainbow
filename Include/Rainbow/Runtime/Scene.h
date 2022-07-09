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
		Scene(Device* pDevice);
		~Scene();
		void LoadModelFromFile(const char* file);
		void Draw();
	private:
		Device* pDevice = nullptr;
		Queue* pQueue = nullptr;
		std::vector<GameObject*> mGameObject;
		std::vector<GameObject*> mRenderableObject;
		std::vector<Component*> mComponent;
	};

	void CreateScene(Device *pDevice, Scene**ppScene);
	void RemoveScene(Scene* pScene);
}