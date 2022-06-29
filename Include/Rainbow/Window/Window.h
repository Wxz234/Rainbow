//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

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

	void CreateRenderWindow(const char* title, uint32_t width, uint32_t height, WNDPROC wndproc, Window** ppWindow);
	void RemoveRenderWindow(Window* pWindow);
	void RenderWindowShow(Window* pWindow);

	template <typename Function, typename... Args>
	void RenderWindowRunLoop(Window* pWindow, Function&& f, Args&&... args) {
		MSG msg{};
		while (msg.message != WM_QUIT) {
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				std::invoke(std::forward<Function>(f), std::forward<Args>(args)...);
			}
		}
	}
}