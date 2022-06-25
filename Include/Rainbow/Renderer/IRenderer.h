//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "../Window/Window.h"
#include "../../../ThirdParty/D3D12MemoryAllocator/include/D3D12MemAlloc.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
#include <handleapi.h>

namespace Rainbow {

	enum QueueType
	{
		QUEUE_TYPE_GRAPHICS = 0,
		QUEUE_TYPE_COPY,
		QUEUE_TYPE_COMPUTE,
	};

	struct Device {
		IDXGIAdapter4* pDxActiveGPU;
		ID3D12Device7* pDxDevice;
		D3D12MA::Allocator* pResourceAllocator;
	};

	void CreateDevice(Device** ppDevice);
	void RemoveDevice(Device* pDevice);

	struct Queue {
		ID3D12CommandQueue* pDxQueue;
		ID3D12Fence* pDxFence;
		HANDLE pDxWaitIdleFenceEvent;
		uint64_t mFenceValue;
		IDXGISwapChain* pSubmitSwapChain;
	};

	struct QueueDesc {
		QueueType mType;
	};

	void CreateQueue(Device* pDevice, QueueDesc* pDesc, Queue** ppQueue);
	void RemoveQueue(Queue* pQueue);

	struct SwapChain {
		IDXGISwapChain4* pDxSwapChain;
		ID3D12DescriptorHeap* pDxRTVHeap;
		uint64_t* pFenceValue;
	};

	struct SwapChainDesc {
		WindowHandle mWindowHandle;
		uint32_t mImageCount;
		uint32_t mWidth;
		uint32_t mHeight;
		DXGI_FORMAT mColorFormat;
	};

	void CreateSwapChain(Queue* pQueue, SwapChainDesc* pDesc, SwapChain** ppSwapChain);
	void RemoveSwapChain(SwapChain* ppSwapChain);

	void QueuePresent(Queue* pQueue, SwapChain* pSwapChain);
}