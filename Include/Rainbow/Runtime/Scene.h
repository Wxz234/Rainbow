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
		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;
		Scene(Scene&&) = delete;
		Scene& operator=(Scene&&) = delete;
		~Scene();
		void LoadModelFromFile(const char* file);
		void UpdateCameraPerspective(float FovAngleY, float AspectRatio, float NearZ, float FarZ);

		void Draw();

	private:
		Device* pDevice;
		Queue* pGQueue;
		Queue* pCQueue;

		std::vector<GameObject*> mGameObject;
		GameObject* mMainCamera;
	};

	void CreateScene(Device *pDevice, Scene**ppScene);
	void RemoveScene(Scene* pScene);
}