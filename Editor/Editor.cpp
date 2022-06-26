//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Renderer/IRenderer.h"
#include "../Include/Rainbow/GUI/GUI.h"

#include "../ThirdParty/imgui/imgui.h"

#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (Rainbow::GUIWndProcHandler(hWnd, message, wParam, lParam))
		return true;
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
	constexpr uint32_t width = 800, height = 600, frameCount = 3;
	
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
	Rainbow::QueueDesc queueDesc = { Rainbow::COMMAND_TYPE_GRAPHICS };
	Rainbow::CreateQueue(pDevice, &queueDesc, &pQueue);
	Rainbow::SwapChain* pSwapChain = nullptr;
	Rainbow::SwapChainDesc swapchainDesc{ hwnd, frameCount, width, height, DXGI_FORMAT_R8G8B8A8_UNORM };
	Rainbow::CreateSwapChain(pQueue, &swapchainDesc, &pSwapChain);
	Rainbow::GUI* pGui = nullptr;
	Rainbow::CreateGUI(pDevice, pSwapChain, &pGui);
	
	Rainbow::CmdPool* pCmdPool[frameCount]{};
	Rainbow::CmdPoolDesc poolDesc{ Rainbow::COMMAND_TYPE_GRAPHICS };
	Rainbow::Cmd* pCmd[frameCount]{};
	for (uint32_t i = 0;i < frameCount; ++ i) {
		Rainbow::CreateCmdPool(pDevice, &poolDesc, &pCmdPool[i]);
		Rainbow::CmdDesc cmdDesc{ Rainbow::COMMAND_TYPE_GRAPHICS, pCmdPool[i] };
		Rainbow::CreateCmd(pDevice, &cmdDesc, &pCmd[i]);
	}

	ShowWindow(hwnd, SW_SHOWDEFAULT);
	MSG msg{};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			auto frameIndex = pSwapChain->pDxSwapChain->GetCurrentBackBufferIndex();
			Rainbow::CmdReset(pCmd[frameIndex]);

			ID3D12Resource* res = nullptr;
			Rainbow::GetSwapChainBuffer(pSwapChain, frameIndex, &res);

			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = res;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			pCmd[frameIndex]->pDxCmdList->ResourceBarrier(1, &barrier);

			auto rtv = Rainbow::GetSwapChainRTV(pSwapChain);
			pCmd[frameIndex]->pDxCmdList->OMSetRenderTargets(1, &rtv, false, nullptr);
			float color[4] = { 0, 1, 0, 1 };
			pCmd[frameIndex]->pDxCmdList->ClearRenderTargetView(rtv, color, 0, nullptr);

			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			pCmd[frameIndex]->pDxCmdList->ResourceBarrier(1, &barrier);

			Rainbow::CmdClose(pCmd[frameIndex]);
			Rainbow::QueueExecute(pQueue, pCmd[frameIndex]);
			Rainbow::QueuePresent(pQueue, pSwapChain);

			res->Release();
		}
	}

	for (uint32_t i = 0; i < frameCount; ++i) {
		Rainbow::RemoveCmdPool(pCmdPool[i]);
		Rainbow::RemoveCmd(pCmd[i]);
	}

	Rainbow::RemoveGUI(pGui);
	Rainbow::RemoveSwapChain(pSwapChain);
	Rainbow::RemoveQueue(pQueue);
	Rainbow::RemoveDevice(pDevice);

	return 0;
}