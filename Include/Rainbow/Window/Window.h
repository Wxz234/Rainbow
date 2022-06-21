//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include <Windows.h>

namespace Rainbow {
	struct WindowHandle {
		void* window;
	};

	class IWindow {
	public:
		virtual bool Init() = 0;
		virtual void Exit() = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void Draw() = 0;
	};
}