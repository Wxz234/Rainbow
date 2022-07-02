//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

namespace Rainbow {
	struct Scene {
		class Impl;
		Impl* pimpl;
	};

	void CreateScene(Scene** ppScene);
	void RemoveScene(Scene* pScene);
}