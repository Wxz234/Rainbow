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

	ID3D12CommandAllocator* mainAllocator[3]{};
	pDevice->pDxDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mainAllocator[0]));
	pDevice->pDxDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mainAllocator[1]));
	pDevice->pDxDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mainAllocator[2]));
	ID3D12GraphicsCommandList* mainList = nullptr;
	pDevice->pDxDevice->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&mainList));
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	MSG msg{};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			Rainbow::NewFrame(pGui);

			bool is_open = true;
			ImGui::Begin("Another Window", &is_open);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			ImGui::End();
		
			ImGui::Render();

			auto frameIndex = pSwapChain->pDxSwapChain->GetCurrentBackBufferIndex();
			ID3D12Resource* rtResource = nullptr;
			pSwapChain->pDxSwapChain->GetBuffer(frameIndex, IID_PPV_ARGS(&rtResource));

			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = rtResource;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			mainAllocator[frameIndex]->Reset();
			mainList->Reset(mainAllocator[frameIndex], nullptr);
			mainList->ResourceBarrier(1, &barrier);

			const float clear_color_with_alpha[4] = { 0, 0, 0, 1 };
			auto rtv = Rainbow::GetSwapChainRTV(pSwapChain);
			mainList->ClearRenderTargetView(rtv, clear_color_with_alpha, 0, nullptr);
			mainList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
			mainList->SetDescriptorHeaps(1, &pGui->pSrvHeap);

			Rainbow::DrawGUI(mainList);

			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			mainList->ResourceBarrier(1, &barrier);
			mainList->Close();

			pQueue->pDxQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&mainList);

			Rainbow::QueuePresent(pQueue, pSwapChain);
			rtResource->Release();

		}
	}

	Rainbow::RemoveGUI(pGui);
	Rainbow::RemoveSwapChain(pSwapChain);
	Rainbow::RemoveQueue(pQueue);
	Rainbow::RemoveDevice(pDevice);

	mainAllocator[0]->Release();
	mainAllocator[1]->Release();
	mainAllocator[2]->Release();
	mainList->Release();

	return 0;
}