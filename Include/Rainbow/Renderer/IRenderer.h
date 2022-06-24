//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "../Window/Window.h"
#include "../ThirdParty/D3D12MemoryAllocator/include/D3D12MemAlloc.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
#include <handleapi.h>

namespace Rainbow {
	enum PipelineType
	{
		PIPELINE_TYPE_GRAPHICS = 0,
		PIPELINE_TYPE_COMPUTE,
		PIPELINE_TYPE_COUNT,
	};

	enum QueueType
	{
		QUEUE_TYPE_GRAPHICS = 0,
		QUEUE_TYPE_COPY,
		QUEUE_TYPE_COMPUTE,
	};

	struct Device {
		IDXGIAdapter4* pDxActiveGPU;
		ID3D12Device* pDxDevice;
		D3D12MA::Allocator* pResourceAllocator;
	};

	void CreateDevice(Device** ppDevice);
	void RemoveDevice(Device* pDevice);

	struct Texture {
		ID3D12Resource* pDxResource;
		D3D12MA::Allocation* pDxAllocation;
	};

	struct TextureDesc {
		D3D12_CLEAR_VALUE mClearValue;
		DXGI_FORMAT mFormat;
		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mArraySize;
		uint32_t mMipLevels;
		D3D12_RESOURCE_STATES mStartState;
		D3D12_RESOURCE_FLAGS mFlags;
	};

	void CreateTexture(Device* pDevice, TextureDesc* pDesc, Texture** ppTexture);
	void RemoveTexture(Texture* pTexture);

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