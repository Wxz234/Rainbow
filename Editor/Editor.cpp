//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Window/Window.h"
#include "../Include/Rainbow/Renderer/IRenderer.h"
#include "../Include/Rainbow/GUI/GUI.h"
#include "../Include/Rainbow/Runtime/Scene.h"

#include "../ThirdParty/imgui/imgui.h"

#include <Windows.h>

uint32_t w = 800, h = 600;
constexpr uint32_t frameCount = 3;
Rainbow::Device* pDevice = nullptr;
Rainbow::SwapChain* pSwapChain = nullptr;
Rainbow::Texture* pTex = nullptr;
Rainbow::GUI* pGui = nullptr;

void Draw() {

	Rainbow::GUINewFrame(pGui);
	ImGui::Begin("test");
	ImGui::End();
	Rainbow::BeginDraw(pDevice, pSwapChain);
	Rainbow::GUIDraw(pGui);
	Rainbow::EndDraw(pDevice, pSwapChain);
	Rainbow::SwapChainPresent(pSwapChain);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (Rainbow::GUIWndProcHandler(hWnd, message, wParam, lParam)) {
		return true;
	}
	switch (message)
	{
	case WM_SIZE:
		w = LOWORD(lParam);
		h = HIWORD(lParam);
		Rainbow::QueueWait(pDevice->pQueue);
		Rainbow::SwapChainResize(pSwapChain, w, h, DXGI_FORMAT_UNKNOWN);
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
	Rainbow::Window* pWindow = nullptr;
	Rainbow::CreateRenderWindow("RainbowEditor", w, h, WndProc, &pWindow);
	Rainbow::CreateDevice(&pDevice);
	Rainbow::SwapChainDesc swapchainDesc{ pWindow->mWindowHandle, frameCount, w, h, DXGI_FORMAT_R8G8B8A8_UNORM };
	Rainbow::CreateSwapChain(pDevice, &swapchainDesc, &pSwapChain);
	Rainbow::CreateGUI(pDevice, pSwapChain, &pGui);

	Rainbow::RenderWindowShow(pWindow);
	Rainbow::RenderWindowRunLoop(pWindow, Draw);

	Rainbow::RemoveGUI(pGui);
	Rainbow::RemoveSwapChain(pSwapChain);
	Rainbow::RemoveDevice(pDevice);
	Rainbow::RemoveRenderWindow(pWindow);
	return 0;
}