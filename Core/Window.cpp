//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Window/Window.h"

#include <cassert>
#include <filesystem>

namespace Rainbow {
	void CreateRenderWindow(const char* title, uint32_t width, uint32_t height, WNDPROC wndproc, Window** ppWindow) {
		assert(title);
		assert(ppWindow);
		Window* pWindow = new Window;
		WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEXW);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = wndproc;
		wcex.hInstance = GetModuleHandle(nullptr);
		wcex.hIcon = LoadIconW(wcex.hInstance, L"IDI_ICON");
		wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wcex.lpszClassName = L"RainbowEditor";
		wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
		RegisterClassExW(&wcex);

		RECT rc = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
		auto stype = WS_OVERLAPPEDWINDOW;
		AdjustWindowRect(&rc, stype, FALSE);
		std::filesystem::path my_title{ title };
		pWindow->mWindowHandle.window = CreateWindowExW(0, L"RainbowEditor", my_title.wstring().c_str(), stype, 10, 10, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
		*ppWindow = pWindow;
	}

	void RemoveRenderWindow(Window* pWindow) {
		assert(pWindow);
		delete pWindow;
	}

	void RenderWindowShow(Window* pWindow) {
		assert(pWindow);
		::ShowWindow((HWND)pWindow->mWindowHandle.window, SW_SHOW);
	}
}