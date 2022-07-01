//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Window/Window.h"
#include "../Include/Rainbow/Renderer/IRenderer.h"
#include "../Include/Rainbow/GUI/GUI.h"

#include "../ThirdParty/imgui/imgui.h"

#include <Windows.h>
#include <string>

uint32_t w = 1024, h = 768;
constexpr uint32_t frameCount = 3;
Rainbow::Device* pDevice = nullptr;
Rainbow::SwapChain* pSwapChain = nullptr;
Rainbow::GUI* pGui = nullptr;

void LoadImguiConfig() {
	std::string file_str = RAINBOW_PATH + std::string("Config/editor_imgui.ini");
	ImGui::LoadIniSettingsFromDisk(file_str.c_str());
}

void Draw() {
	Rainbow::GUINewFrame(pGui);
	ImGui::Begin("Scene");
	ImGui::End();
	ImGui::Begin("Details");
	ImGui::End();
	ImGui::Begin("Content Browser");
	ImGui::End();
	ImGui::Begin("Game");
	ImGui::End();

	Rainbow::BeginDraw(pDevice, pSwapChain);

	auto activeList = Rainbow::GetSwapChainActiveList(pSwapChain);
	auto rtv = Rainbow::GetSwapChainRTV(pSwapChain);
	float color[4]{ 0, 0, 0, 1 };
	activeList->pDxCmdList->ClearRenderTargetView(rtv, color, 0, 0);
	activeList->pDxCmdList->OMSetRenderTargets(1, &rtv, 0, 0);
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
	Rainbow::CreateGUI(pDevice, pSwapChain,&pGui);
	LoadImguiConfig();

	Rainbow::Texture* pTex;
	Rainbow::CreateTextureFromFile(pDevice, "C:\\Users\\42937\\Documents\\GitHub\\Rainbow\\Asset\\CornellBox.png", &pTex);

	Rainbow::RenderWindowShow(pWindow);
	Rainbow::RenderWindowRunLoop(pWindow, Draw);

	Rainbow::RemoveTexture(pTex);
	Rainbow::RemoveGUI(pGui);
	Rainbow::RemoveSwapChain(pSwapChain);
	Rainbow::RemoveDevice(pDevice);
	Rainbow::RemoveRenderWindow(pWindow);
	return 0;
}
