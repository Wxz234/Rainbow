//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "../Include/Rainbow/Runtime/SubMesh.h"

#include <cstdint>

namespace Rainbow {
	struct GLTFLoader {
		GLTFLoader(const char *file);
		~GLTFLoader();

		GLTFLoader(const GLTFLoader&) = delete;
		GLTFLoader& operator=(const GLTFLoader&) = delete;
		GLTFLoader(GLTFLoader&&) = delete;
		GLTFLoader& operator=(GLTFLoader&&) = delete;

		uint32_t GetSubMeshSize() const;
		SubMesh GetSubMesh(uint32_t i) const;
	private:
		class Impl;
		Impl* pimpl;
	};
}