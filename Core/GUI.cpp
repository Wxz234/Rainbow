//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/GUI/GUI.h"

#include "../ThirdParty/imgui/imgui.h"

#include <cassert>
#include <combaseapi.h>

// imgui
bool ImGui_ImplWin32_Init(void* hwnd);
bool ImGui_ImplDX12_Init(
	ID3D12Device* device,
	int num_frames_in_flight,
	DXGI_FORMAT rtv_format,
	ID3D12DescriptorHeap* cbv_srv_heap,
	D3D12_CPU_DESCRIPTOR_HANDLE font_srv_cpu_desc_handle,
	D3D12_GPU_DESCRIPTOR_HANDLE font_srv_gpu_desc_handle
);
void ImGui_ImplDX12_Shutdown();
void ImGui_ImplWin32_Shutdown();

namespace Rainbow {
	void CreateGUI(Device* pDevice, SwapChain* pSwapChain, GUI** ppGui) {
		assert(pDevice);
		assert(pSwapChain);
		assert(ppGui);
		GUI* pGui = new GUI;

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.NumDescriptors = 1;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		pDevice->pDxDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&pGui->pSrvHeap));

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();

		HWND hwnd;
		pSwapChain->pDxSwapChain->GetHwnd(&hwnd);

		ImGui_ImplWin32_Init(hwnd);

		DXGI_SWAP_CHAIN_DESC swapchainDesc{};
		pSwapChain->pDxSwapChain->GetDesc(&swapchainDesc);
		ImGui_ImplDX12_Init(
			pDevice->pDxDevice,
			swapchainDesc.BufferCount,
			swapchainDesc.BufferDesc.Format,
			pGui->pSrvHeap,
			pGui->pSrvHeap->GetCPUDescriptorHandleForHeapStart(),
			pGui->pSrvHeap->GetGPUDescriptorHandleForHeapStart()
		);
		*ppGui = pGui;
	}
	void RemoveGUI(GUI* pGui) {
		assert(pGui);
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		pGui->pSrvHeap->Release();
		delete pGui;
	}
}