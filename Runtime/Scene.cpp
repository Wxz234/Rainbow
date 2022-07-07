//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Runtime/Scene.h"
#include "../Include/Rainbow/Runtime/ModelObject.h"
#include "../Include/Rainbow/Runtime/MeshComponent.h"

#include "../ThirdParty/json/json.hpp"

#include <filesystem>
#include <fstream>
#include <cassert>
#include <vector>
#include <string>
#include <Urlmon.h>

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



	void _get_buffer(std::vector<unsigned char>& buffer, nlohmann::json& buffer_json) {
		auto uri_str = buffer_json["uri"].get<std::string>();
		std::filesystem::path uri_wstr{ uri_str };
		IUri* pUri = nullptr;
		CreateUri(L"data:application/octet-stream;base64,abcdagyhubcd+bzdtbcdab+d", Uri_CREATE_CANONICALIZE, 0, &pUri);
		if (pUri) {
			throw;
			
			pUri->Release();
			return;
		}
	}

	void Scene::LoadModelFromFile(const char* file) {
		std::ifstream i(file);
		std::filesystem::path my_path{ file };
		nlohmann::json gltf;
		i >> gltf;

		ModelObject* mo = new ModelObject;
		auto meshcomponent = dynamic_cast<MeshComponent*>(mo->GetComponent(0));
		auto mesh_size = gltf["meshes"].size();
		for (size_t mesh_index = 0; mesh_index < mesh_size; ++mesh_index) {
			auto primitive_size = gltf["meshes"][mesh_index]["primitives"].size();
			for (size_t primitive_index = 0; primitive_index < primitive_size; ++primitive_index) {
				//gltf["meshes"][mesh_index]["primitives"][primitive_index]["POSITION"]
				auto attribute = gltf["meshes"][mesh_index]["primitives"][primitive_index]["attributes"];
				auto position_index = attribute["POSITION"].get<int>();
				auto bufferview_index = gltf["accessors"][position_index]["bufferView"].get<int>();
				auto buffer_index = gltf["bufferViews"][bufferview_index]["buffer"].get<int>();
				auto buffers = gltf["buffers"][buffer_index];
				std::vector<unsigned char> bufferData;
				_get_buffer(bufferData, buffers);
			}
			//meshcomponent->submesh.push_back()
		}

		mGameObject.push_back(mo);

	}  
}