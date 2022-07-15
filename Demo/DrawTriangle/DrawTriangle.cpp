//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../../Include/Rainbow/Window/Window.h"
#include "../../Include/Rainbow/Renderer/IRenderer.h"

#include <Windows.h>

uint32_t w = 1024, h = 768;
constexpr uint32_t frameCount = 3;
Rainbow::Window* pWindow = nullptr;
Rainbow::Device* pDevice = nullptr;
Rainbow::SwapChain* pSwapChain = nullptr;
Rainbow::Shader* pVSShader = nullptr;
Rainbow::Shader* pPSShader = nullptr;

void Draw() {
	Rainbow::BeginDraw(pSwapChain);
	Rainbow::EndDraw(pSwapChain);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		if (SIZE_MINIMIZED != wParam) {
			w = LOWORD(lParam);
			h = HIWORD(lParam);
			Rainbow::QueueWait(pDevice->pQueue);
			Rainbow::SwapChainResize(pSwapChain, w, h, DXGI_FORMAT_UNKNOWN);
		}
		break;

	case WM_GETMINMAXINFO:
		if (lParam) {
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
	Rainbow::CreateRenderWindow("RainbowEditor", w, h, WndProc, &pWindow);
	Rainbow::CreateDevice(&pDevice);
	Rainbow::SwapChainDesc swapchainDesc{ pWindow->mWindowHandle, frameCount, w, h, DXGI_FORMAT_R8G8B8A8_UNORM };
	Rainbow::CreateSwapChain(pDevice, &swapchainDesc, &pSwapChain);

	Rainbow::ShaderDesc shaderDesc{ Rainbow::SHADER_STAGE_VERTEX ,"main" };
	Rainbow::CreateShaderFromFile(pDevice, "Shader/02VertexShader.hlsl", &shaderDesc, &pVSShader);
	shaderDesc.mStages = Rainbow::SHADER_STAGE_PIXEL;
	Rainbow::CreateShaderFromFile(pDevice, "Shader/02PixelShader.hlsl", &shaderDesc, &pPSShader);

	Rainbow::RenderWindowShow(pWindow);
	Rainbow::RenderWindowRunLoop(pWindow, Draw);

	Rainbow::RemoveShader(pVSShader);
	Rainbow::RemoveShader(pPSShader);
	Rainbow::RemoveSwapChain(pSwapChain);
	Rainbow::RemoveDevice(pDevice);
	Rainbow::RemoveRenderWindow(pWindow);
	return 0;
}
