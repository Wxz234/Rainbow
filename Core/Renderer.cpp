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

	void CreateTexture(Device* pDevice, TextureDesc* pDesc, Texture** ppTexture) {
		assert(pDevice);
		assert(pDesc);
		assert(ppTexture);
		D3D12_RESOURCE_DESC texDesc{ D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, pDesc->mWidth, pDesc->mHeight, pDesc->mArraySize, pDesc->mMipLevels, pDesc->mFormat, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, pDesc->mFlags };

		D3D12MA::ALLOCATION_DESC allocDesc{};
		allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		Texture* pTexture = new Texture;

		D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr;
		if ((pDesc->mFlags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) || (pDesc->mFlags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL))
		{
			pOptimizedClearValue = &pDesc->mClearValue;
		}
		pDevice->pResourceAllocator->CreateResource(&allocDesc, &texDesc, pDesc->mStartState, pOptimizedClearValue, &pTexture->pDxAllocation, IID_PPV_ARGS(&pTexture->pDxResource));

		*ppTexture = pTexture;
	}
	void RemoveTexture(Texture* pTexture) {
		assert(pTexture);
		pTexture->pDxAllocation->Release();
		pTexture->pDxResource->Release();
		delete pTexture;
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
		*ppQueue = pQueue;
	}

	void RemoveQueue(Queue* pQueue) {
		assert(pQueue);
		pQueue->pDxQueue->Release();
		delete pQueue;
	}
}