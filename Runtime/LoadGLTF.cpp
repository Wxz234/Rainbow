//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../ThirdParty/json/json.hpp"

#include "LoadGLTF.h"

#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

namespace Rainbow {
	class GLTFLoader::Impl {
	public:
		nlohmann::json gltf;
		std::vector<std::vector<unsigned char>> m_all_buffer;
		std::filesystem::path parent_path;
	};

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

		while (in_len-- && (encoded_string[in_] != '=')) {
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
	
	GLTFLoader::GLTFLoader(const char* file) {
		pimpl = new Impl;
		std::ifstream i(file);
		i >> pimpl->gltf;
		auto& gltf = pimpl->gltf;

		pimpl->parent_path = std::filesystem::path{ file }.parent_path();

		auto buffers = gltf["buffers"];
		for (size_t i = 0;i < buffers.size(); ++i) {
			auto uri = buffers[i]["uri"].get<std::string>();
			auto byteLength = buffers[i]["byteLength"].get<int>();
			std::vector<unsigned char> bufferData;
			if(uri.starts_with("data:application/octet-stream;base64,")){
				auto decode_data = _base64_decode(std::string(uri.begin() + 37, uri.end()));

				for (size_t i = 0;i < decode_data.size(); ++i) {
					bufferData.push_back(static_cast<unsigned char>(decode_data[i]));
				}
			}
			else {
				auto bin_path_str = pimpl->parent_path.string() + "\\" + uri;
				std::ifstream ifs(bin_path_str.c_str(), std::ios::binary);
				bufferData.resize(byteLength);
				ifs.read((char*)bufferData.data(), byteLength);
			}

			pimpl->m_all_buffer.push_back(bufferData);
		}
	}

	GLTFLoader::~GLTFLoader() {
		delete pimpl;
	}
}