//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Window/Window.h"
#include "../Include/Rainbow/Renderer/IRenderer.h"

#include <Windows.h>

void Draw() {
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		if (SIZE_MINIMIZED != wParam) {
		}
		break;

	case WM_GETMINMAXINFO:
		if (lParam)
		{
			auto info = reinterpret_cast<MINMAXINFO*>(lParam);
			info->ptMinTrackSize.x = 320;
			info->ptMinTrackSize.y = 200;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	return 0;
}
