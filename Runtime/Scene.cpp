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

#include <fstream>
#include <cassert>
#include <vector>
#include <string>
#include <cctype>
#include <filesystem>

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

	bool _is_base64(unsigned char c) {
		return (std::isalnum(c) || (c == '+') || (c == '/'));
	}

	std::string _base64_decode(const std::string& encoded_string) {

		const std::string base64_chars =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";

		size_t in_len = encoded_string.size();
		int i = 0;
		int j = 0;
		int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];
		std::string ret;

		while (in_len-- && (encoded_string[in_] != '=') && _is_base64(encoded_string[in_])) {
			char_array_4[i++] = encoded_string[in_]; in_++;
			if (i == 4) {
				for (i = 0; i < 4; i++)
					char_array_4[i] = base64_chars.find(char_array_4[i]) & 0xff;

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (i = 0; (i < 3); i++)
					ret += char_array_3[i];
				i = 0;
			}
		}

		if (i) {
			for (j = 0; j < i; j++)
				char_array_4[j] = base64_chars.find(char_array_4[j]) & 0xff;

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

			for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
		}

		return ret;
	}

	void _get_buffer(const char* file, std::vector<unsigned char>& buffer, nlohmann::json& buffer_json) {
		auto uri_str = buffer_json["uri"].get<std::string>();
		buffer_json["byteLength"].get<int>();
		//if (uri_str.starts_with("data:application/octet-stream;base64,")) {
		//	std::string base_str(uri_str.begin() + 37, uri_str.end());
		//	auto out_put = _base64_decode(base_str);
		//	for (auto iter = out_put.begin(); iter != out_put.end(); ++iter) {
		//		buffer.push_back(static_cast<unsigned char>(*iter));
		//	}
		//	return;
		//}

		//std::filesystem::path my_dir{ file };
		//auto parent_dir = my_dir.parent_path();
		//auto buffer_path = parent_dir.string() + "\\" + uri_str;

		//std::ifstream i(buffer_path);
		//auto file_size = i.tellg();
		//buffer.resize(file_size);

		//i.close();

	}

	void Scene::LoadModelFromFile(const char* file) {
		std::ifstream i(file);
		nlohmann::json gltf;
		i >> gltf;
		
		ModelObject* mo = new ModelObject;
		auto meshcomponent = dynamic_cast<MeshComponent*>(mo->GetComponent(0));
		auto mesh_size = gltf["meshes"].size();
		for (size_t mesh_index = 0; mesh_index < mesh_size; ++mesh_index) {
			auto primitive_size = gltf["meshes"][mesh_index]["primitives"].size();
			for (size_t primitive_index = 0; primitive_index < primitive_size; ++primitive_index) {

				auto attribute = gltf["meshes"][mesh_index]["primitives"][primitive_index]["attributes"];
				auto position_index = attribute["POSITION"].get<int>();
				auto bufferview_index = gltf["accessors"][position_index]["bufferView"].get<int>();
				auto buffer_index = gltf["bufferViews"][bufferview_index]["buffer"].get<int>();
				auto buffers = gltf["buffers"][buffer_index];
				std::vector<unsigned char> bufferData;
				_get_buffer(file, bufferData, buffers);
			}
		}

		i.close();
		mGameObject.push_back(mo);
	}  
}