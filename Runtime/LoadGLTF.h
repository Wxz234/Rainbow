//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

namespace Rainbow {
	struct GLTFLoader {
		GLTFLoader(const char *file);
		~GLTFLoader();
	private:
		class Impl;
		Impl* pimpl;
	};
}