//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "Rainbow/Renderer/IRenderer.h"
#include "Rainbow/GUI/GUI.h"

#include <Windows.h>
#include <d3d12.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
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
	uint32_t width = 800, height = 600;

	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = GetModuleHandle(nullptr);
	wcex.hIcon = LoadIconW(wcex.hInstance, L"IDI_ICON");
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wcex.lpszClassName = L"RainbowEditor";
	wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
	RegisterClassExW(&wcex);

	RECT rc = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	auto stype = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;
	AdjustWindowRect(&rc, stype, FALSE);
	auto hwnd = CreateWindowExW(0, L"RainbowEditor", L"RainbowEditor", stype, 50, 50, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

	Rainbow::Device* pDevice = nullptr;
	Rainbow::CreateDevice(&pDevice);
	Rainbow::Queue* pQueue = nullptr;
	Rainbow::QueueDesc queueDesc = { Rainbow::QUEUE_TYPE_GRAPHICS };
	Rainbow::CreateQueue(pDevice, &queueDesc, &pQueue);
	Rainbow::SwapChain* pSwapChain = nullptr;
	Rainbow::SwapChainDesc swapchainDesc{ hwnd, 3, width, height, DXGI_FORMAT_R8G8B8A8_UNORM };
	Rainbow::CreateSwapChain(pQueue, &swapchainDesc, &pSwapChain);
	Rainbow::GUI* pGui = nullptr;
	Rainbow::CreateGUI(pDevice, pSwapChain, &pGui);
	

	ShowWindow(hwnd, SW_SHOWDEFAULT);
	MSG msg{};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {


			Rainbow::QueuePresent(pQueue, pSwapChain);
		}
	}

	Rainbow::RemoveGUI(pGui);
	Rainbow::RemoveSwapChain(pSwapChain);
	Rainbow::RemoveQueue(pQueue);
	Rainbow::RemoveDevice(pDevice);

	return 0;
}