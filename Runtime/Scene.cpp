//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Runtime/Scene.h"
#include "../Include/Rainbow/Runtime/ModelObject.h"
#include "../Include/Rainbow/Runtime/MeshComponent.h"

#include "LoadGLTF.h"

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
		for (auto& go : pScene->mGameObject) {
			delete go;
		}
		delete pScene;
	}

	void Scene::LoadModelFromFile(const char* file) {
		GLTFLoader loader(file);
		
		ModelObject* mo = new ModelObject;
		mGameObject.push_back(mo);
	}  
}