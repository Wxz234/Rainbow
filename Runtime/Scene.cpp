//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Runtime/Scene.h"
#include "../Include/Rainbow/Runtime/ModelObject.h"
#include "../Include/Rainbow/Runtime/CameraObject.h"

#include "LoadGLTF.h"

#include <cassert>

namespace Rainbow {
	void CreateScene(Device* pDevice, Scene** ppScene) {
		assert(pDevice);
		assert(ppScene);
		Scene* pScene = new Scene(pDevice);
		*ppScene = pScene;
	}
	void RemoveScene(Scene* pScene) {
		assert(pScene);	
		delete pScene;
	}

	Scene::Scene(Device* pDevice) {
		this->pDevice = pDevice;
		pGQueue = pDevice->pQueue;
		CreateQueue(pDevice, COMMAND_TYPE_COMPUTE, &pCQueue);
		mMainCamera = new CameraObject;

	}

	Scene::~Scene() {
		for (auto &x: mGameObject) {
			delete x;
		}
		delete mMainCamera;

		RemoveQueue(pCQueue, true);
	}

	void Scene::LoadModelFromFile(const char* file) {
		GLTFLoader loader(file);
		auto submesh_size = loader.GetSubMeshSize();

		ModelObject* mo = new ModelObject;
		for (uint32_t i = 0;i < submesh_size; ++i) {
			mo->AddSubMesh(loader.GetSubMesh(i));
		}

		mGameObject.push_back(mo);
	}

	void Scene::UpdateCameraPerspective(float FovAngleY, float AspectRatio, float NearZ, float FarZ) {
		((CameraObject*)mMainCamera)->UpdatePerspective(FovAngleY, AspectRatio, NearZ, FarZ);
	}

	void Scene::Draw() {

	}
}