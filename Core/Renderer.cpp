//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "Rainbow/Renderer/IRenderer.h"

#include <d3d12sdklayers.h>
#include <winerror.h>
#include <combaseapi.h>
#include <synchapi.h>
#include <cassert>

#pragma warning (disable: 6011)
#pragma warning (disable: 4838)

namespace Rainbow {
	void CreateDevice(Device** ppDevice) {
		assert(ppDevice);

		Device* pDevice = new Device;
		IDXGIFactory7* temp_factory = nullptr;
#ifdef _DEBUG
		ID3D12Debug* debugController = nullptr;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			debugController->Release();
		}

		CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&temp_factory));
#else
		CreateDXGIFactory2(0, IID_PPV_ARGS(&temp_factory));
#endif // DEBUG
		temp_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&pDevice->pDxActiveGPU));
		temp_factory->Release();
		D3D12CreateDevice(nullptr, (D3D_FEATURE_LEVEL)0xc200, IID_PPV_ARGS(&pDevice->pDxDevice));

		D3D12MA::ALLOCATOR_DESC desc = {};
		desc.Flags = D3D12MA::ALLOCATOR_FLAG_NONE;
		desc.pDevice = pDevice->pDxDevice;
		desc.pAdapter = pDevice->pDxActiveGPU;
		D3D12MA::CreateAllocator(&desc, &pDevice->pResourceAllocator);

		*ppDevice = pDevice;
	}

	void RemoveDevice(Device* pDevice) {
		assert(pDevice);
		pDevice->pResourceAllocator->Release();
		pDevice->pDxDevice->Release();
		pDevice->pDxActiveGPU->Release();
		delete pDevice;
	}

	void CreateQueue(Device* pDevice, QueueDesc* pDesc, Queue** ppQueue) {
		assert(pDevice);
		assert(pDesc);
		assert(ppQueue);
		Queue* pQueue = new Queue;
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		if (pDesc->mType == QUEUE_TYPE_GRAPHICS) {
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		}
		else if (pDesc->mType == QUEUE_TYPE_COMPUTE) {
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		}
		else if (pDesc->mType == QUEUE_TYPE_COPY) {
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		}
		pDevice->pDxDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pQueue->pDxQueue));
		pDevice->pDxDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pQueue->pDxFence));
		pQueue->pDxWaitIdleFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		pQueue->mFenceValue = 1;
		pQueue->pSubmitSwapChain = nullptr;
		*ppQueue = pQueue;
	}

	void RemoveQueue(Queue* pQueue) {
		assert(pQueue);
		const uint64_t fence = pQueue->mFenceValue;
		pQueue->pDxQueue->Signal(pQueue->pDxFence, fence);
		if (pQueue->pDxFence->GetCompletedValue() < fence)
		{
			pQueue->pDxFence->SetEventOnCompletion(fence, pQueue->pDxWaitIdleFenceEvent);
			WaitForSingleObject(pQueue->pDxWaitIdleFenceEvent, INFINITE);
		}
		if (pQueue->pSubmitSwapChain) {
			pQueue->pSubmitSwapChain->Release();
		}
		pQueue->pDxFence->Release();
		pQueue->pDxQueue->Release();
		CloseHandle(pQueue->pDxWaitIdleFenceEvent);
		delete pQueue;
	}

	void CreateSwapChain(Queue* pQueue, SwapChainDesc* pDesc, SwapChain** ppSwapChain) {
		assert(pQueue);
		assert(pDesc);
		assert(ppSwapChain);
		SwapChain* pSwapChain = new SwapChain;

		DXGI_SWAP_CHAIN_DESC1 _desc{};
		_desc.BufferCount = pDesc->mImageCount;
		_desc.Width = pDesc->mWidth;
		_desc.Height = pDesc->mHeight;
		_desc.Format = pDesc->mColorFormat;
		_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		_desc.SampleDesc.Count = 1;
		_desc.SampleDesc.Quality = 0;
		_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		_desc.Scaling = DXGI_SCALING_STRETCH;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc{};
		fsSwapChainDesc.Windowed = TRUE;

		IDXGIFactory7* temp_factory = nullptr;
#ifdef _DEBUG
		CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&temp_factory));
#else
		CreateDXGIFactory2(0, IID_PPV_ARGS(&temp_factory));
#endif
		IDXGISwapChain1* temp_swapchain = nullptr;
		temp_factory->CreateSwapChainForHwnd(pQueue->pDxQueue, (HWND)pDesc->mWindowHandle.window, &_desc, &fsSwapChainDesc, nullptr, &temp_swapchain);
		temp_swapchain->QueryInterface(&pSwapChain->pDxSwapChain);
		temp_factory->MakeWindowAssociation((HWND)pDesc->mWindowHandle.window, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
		
		temp_factory->Release();
		temp_swapchain->Release();

		pSwapChain->pDxSwapChain->QueryInterface(&pQueue->pSubmitSwapChain);
		pSwapChain->pFenceValue = new uint64_t[pDesc->mImageCount]{};

		*ppSwapChain = pSwapChain;
	}

	void RemoveSwapChain(SwapChain* pSwapChain) {
		assert(pSwapChain);
		pSwapChain->pDxSwapChain->Release();
		delete[]pSwapChain->pFenceValue;
		delete pSwapChain;
	}

	void QueuePresent(Queue* pQueue, SwapChain* pSwapChain) {
		assert(pQueue->pSubmitSwapChain == pSwapChain->pDxSwapChain);

		auto frameIndex = pSwapChain->pDxSwapChain->GetCurrentBackBufferIndex();
		pSwapChain->pFenceValue[frameIndex] = pQueue->mFenceValue++;

		pSwapChain->pDxSwapChain->Present(1, 0);

		const uint64_t currentFenceValue = pSwapChain->pFenceValue[frameIndex];
		pQueue->pDxQueue->Signal(pQueue->pDxFence, currentFenceValue);

		frameIndex = pSwapChain->pDxSwapChain->GetCurrentBackBufferIndex();

		if (pQueue->pDxFence->GetCompletedValue() < pSwapChain->pFenceValue[frameIndex])
		{
			pQueue->pDxFence->SetEventOnCompletion(pSwapChain->pFenceValue[frameIndex], pQueue->pDxWaitIdleFenceEvent);
			WaitForSingleObjectEx(pQueue->pDxWaitIdleFenceEvent, INFINITE, FALSE);
		}
	}
}