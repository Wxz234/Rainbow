//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "../Core/Core.h"

#include <Windows.h>
#include <cstdint>
#include <functional>
#include <utility>

namespace Rainbow {
	struct WindowHandle {
		void* window;
	};

	struct Window {
		WindowHandle mWindowHandle;
	};

	RAINBOW_EXPORT void CreateRenderWindow(const char* title, uint32_t width, uint32_t height, WNDPROC wndproc, Window** ppWindow);
	RAINBOW_EXPORT void RemoveRenderWindow(Window* pWindow);
	RAINBOW_EXPORT void RenderWindowShow(Window* pWindow);

	template <typename Function, typename... Args>
	void RenderWindowRunLoop(Window* pWindow, Function&& f, Args&&... args) {
		MSG msg{};
		while (msg.message != WM_QUIT) {
			if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
			else {
				std::invoke(std::forward<Function>(f), std::forward<Args>(args)...);
			}
		}
	}
}