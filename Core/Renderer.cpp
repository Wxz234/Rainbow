//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Renderer/IRenderer.h"

#include "../ThirdParty/D3D12MemoryAllocator/include/D3D12MemAlloc.h"
#include "../ThirdParty/DirectXTex/DDSTextureLoader/DDSTextureLoader12.h"
#include "../ThirdParty/DirectXTex/WICTextureLoader/WICTextureLoader12.h"

#include <d3d12sdklayers.h>
#include <winerror.h>
#include <combaseapi.h>
#include <synchapi.h>
#include <cassert>
#include <string>
#include <filesystem>

#pragma warning (disable: 4838 6011)

namespace Rainbow {
	void CreateDevice(Device** ppDevice) {
		assert(ppDevice);

		CoInitialize(NULL);

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

		QueueDesc queueDesc{ COMMAND_TYPE_GRAPHICS };
		Queue* pQueue = nullptr;
		CreateQueue(pDevice, &queueDesc, &pQueue);
		pDevice->pQueue = pQueue;
		
		CmdPoolDesc poolDesc{ COMMAND_TYPE_GRAPHICS };
		CreateCmdPool(pDevice, &poolDesc, &pDevice->pPool);
		CmdDesc cmdDesc{ COMMAND_TYPE_GRAPHICS, pDevice->pPool };
		CreateCmd(pDevice, &cmdDesc, &pDevice->pCmd);
		*ppDevice = pDevice;
	}

	void RemoveDevice(Device* pDevice) {
		assert(pDevice);
		RemoveQueue(pDevice->pQueue);
		RemoveCmdPool(pDevice->pPool);
		RemoveCmd(pDevice->pCmd);
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
		if (pDesc->mType == COMMAND_TYPE_GRAPHICS) {
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		}
		else if (pDesc->mType == COMMAND_TYPE_GRAPHICS) {
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		}
		else if (pDesc->mType == COMMAND_TYPE_GRAPHICS) {
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

	void QueueWait(Queue* pQueue) {
		assert(pQueue);
		const uint64_t fence = pQueue->mFenceValue++;
		pQueue->pDxQueue->Signal(pQueue->pDxFence, fence);
		if (pQueue->pDxFence->GetCompletedValue() < fence)
		{
			pQueue->pDxFence->SetEventOnCompletion(fence, pQueue->pDxWaitIdleFenceEvent);
			WaitForSingleObject(pQueue->pDxWaitIdleFenceEvent, INFINITE);
		}
	}

	void CreateSwapChain(Device* pDevice, SwapChainDesc* pDesc, SwapChain** ppSwapChain) {
		assert(pDevice);
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
		temp_factory->CreateSwapChainForHwnd(pDevice->pQueue->pDxQueue, (HWND)pDesc->mWindowHandle.window, &_desc, &fsSwapChainDesc, nullptr, &temp_swapchain);
		temp_swapchain->QueryInterface(&pSwapChain->pDxSwapChain);
		temp_factory->MakeWindowAssociation((HWND)pDesc->mWindowHandle.window, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
		
		temp_factory->Release();
		temp_swapchain->Release();
		// rtv
		//ID3D12Device* temp_device = nullptr;

		D3D12_DESCRIPTOR_HEAP_DESC heapdesc = {};
		heapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapdesc.NumDescriptors = pDesc->mImageCount;
		heapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		pDevice->pDxDevice->CreateDescriptorHeap(&heapdesc, IID_PPV_ARGS(&pSwapChain->pDxRTVHeap));

		pSwapChain->mDescriptorSize = pDevice->pDxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = pSwapChain->pDxRTVHeap->GetCPUDescriptorHandleForHeapStart();
		for (uint32_t i = 0; i < pDesc->mImageCount; i++)
		{
			ID3D12Resource* pBackBuffer = nullptr;
			pSwapChain->pDxSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
			pDevice->pDxDevice->CreateRenderTargetView(pBackBuffer, NULL, rtvHandle);
			rtvHandle.ptr += pSwapChain->mDescriptorSize;
			pBackBuffer->Release();
		}

		pSwapChain->pDxSwapChain->QueryInterface(&pDevice->pQueue->pSubmitSwapChain);
		pSwapChain->pFenceValue = new uint64_t[pDesc->mImageCount]{};
		pSwapChain->pDevice = pDevice;
		pSwapChain->cmdPool.resize(pDesc->mImageCount);
		pSwapChain->cmd.resize(pDesc->mImageCount);
		CmdPoolDesc poolDesc{ COMMAND_TYPE_GRAPHICS };
		for (uint32_t i = 0;i < pDesc->mImageCount; ++i) {
			CreateCmdPool(pSwapChain->pDevice, &poolDesc, &(pSwapChain->cmdPool[i]));
			CmdDesc cmdDesc{ COMMAND_TYPE_GRAPHICS, pSwapChain->cmdPool[i] };
			CreateCmd(pSwapChain->pDevice, &cmdDesc, &(pSwapChain->cmd[i]));
		}
		*ppSwapChain = pSwapChain;
	}

	void RemoveSwapChain(SwapChain* pSwapChain) {
		assert(pSwapChain);
		pSwapChain->pDxSwapChain->Release();
		pSwapChain->pDxRTVHeap->Release();
		delete[]pSwapChain->pFenceValue;
		for (uint32_t i = 0; i < pSwapChain->cmdPool.size(); ++i) {
			RemoveCmdPool(pSwapChain->cmdPool[i]);
			RemoveCmd(pSwapChain->cmd[i]);
		}
		delete pSwapChain;
	}

	Cmd* GetSwapChainActiveList(SwapChain* pSwapChain) {
		auto frameIndex = pSwapChain->pDxSwapChain->GetCurrentBackBufferIndex();
		return pSwapChain->cmd[frameIndex];
	}

	void SwapChainResize(SwapChain* pSwapChain, uint32_t width, uint32_t height, DXGI_FORMAT format) {
		assert(pSwapChain);

		DXGI_SWAP_CHAIN_DESC scDesc{};
		pSwapChain->pDxSwapChain->GetDesc(&scDesc);

		pSwapChain->pDxSwapChain->ResizeBuffers(scDesc.BufferCount, width, height, format, 0);
		// rtv

		ID3D12Device* temp_device = nullptr;
		pSwapChain->pDxRTVHeap->GetDevice(IID_PPV_ARGS(&temp_device));
		D3D12_DESCRIPTOR_HEAP_DESC heapdesc = {};
		heapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapdesc.NumDescriptors = scDesc.BufferCount;
		heapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		pSwapChain->pDxRTVHeap->Release();

		temp_device->CreateDescriptorHeap(&heapdesc, IID_PPV_ARGS(&pSwapChain->pDxRTVHeap));

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = pSwapChain->pDxRTVHeap->GetCPUDescriptorHandleForHeapStart();
		for (uint32_t i = 0; i < scDesc.BufferCount; i++)
		{
			ID3D12Resource* pBackBuffer = nullptr;
			pSwapChain->pDxSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
			temp_device->CreateRenderTargetView(pBackBuffer, NULL, rtvHandle);
			rtvHandle.ptr += pSwapChain->mDescriptorSize;
			pBackBuffer->Release();
		}

		temp_device->Release();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetSwapChainRTV(SwapChain* pSwapChain) {
		auto frameIndex = pSwapChain->pDxSwapChain->GetCurrentBackBufferIndex();
		auto rtvHandle = pSwapChain->pDxRTVHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += (pSwapChain->mDescriptorSize * frameIndex);
		return rtvHandle;
	}

	void SwapChainPresent(SwapChain* pSwapChain) {
		assert(pSwapChain);
		auto frameIndex = pSwapChain->pDxSwapChain->GetCurrentBackBufferIndex();
		pSwapChain->pFenceValue[frameIndex] = pSwapChain->pDevice->pQueue->mFenceValue++;

		pSwapChain->pDxSwapChain->Present(1, 0);

		const uint64_t currentFenceValue = pSwapChain->pFenceValue[frameIndex];
		pSwapChain->pDevice->pQueue->pDxQueue->Signal(pSwapChain->pDevice->pQueue->pDxFence, currentFenceValue);

		frameIndex = pSwapChain->pDxSwapChain->GetCurrentBackBufferIndex();

		if (pSwapChain->pDevice->pQueue->pDxFence->GetCompletedValue() < pSwapChain->pFenceValue[frameIndex])
		{
			pSwapChain->pDevice->pQueue->pDxFence->SetEventOnCompletion(pSwapChain->pFenceValue[frameIndex], pSwapChain->pDevice->pQueue->pDxWaitIdleFenceEvent);
			WaitForSingleObjectEx(pSwapChain->pDevice->pQueue->pDxWaitIdleFenceEvent, INFINITE, FALSE);
		}
	}

	void CreateCmdPool(Device* pDevice, CmdPoolDesc* pDesc, CmdPool** ppCmdPool) {
		assert(pDevice);
		assert(pDesc);
		assert(ppCmdPool);

		CmdPool* pCmdPool = new CmdPool;

		D3D12_COMMAND_LIST_TYPE listDesc{};
		if (pDesc->mType == COMMAND_TYPE_GRAPHICS) {
			listDesc = D3D12_COMMAND_LIST_TYPE_DIRECT;
		}
		else if (pDesc->mType == COMMAND_TYPE_GRAPHICS) {
			listDesc = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		}
		else if (pDesc->mType == COMMAND_TYPE_GRAPHICS) {
			listDesc = D3D12_COMMAND_LIST_TYPE_COPY;
		}
		pDevice->pDxDevice->CreateCommandAllocator(listDesc, IID_PPV_ARGS(&pCmdPool->pDxCmdAlloc));

		*ppCmdPool = pCmdPool;
	}
	void RemoveCmdPool(CmdPool* pCmdPool) {
		assert(pCmdPool);
		pCmdPool->pDxCmdAlloc->Release();
		delete pCmdPool;
	}

	void CreateCmd(Device* pDevice, CmdDesc* pDesc, Cmd** ppCmd) {
		assert(pDevice);
		assert(pDesc);
		assert(ppCmd);
		Cmd* pCmd = new Cmd;
		D3D12_COMMAND_LIST_TYPE listDesc{};
		if (pDesc->mType == COMMAND_TYPE_GRAPHICS) {
			listDesc = D3D12_COMMAND_LIST_TYPE_DIRECT;
		}
		else if (pDesc->mType == COMMAND_TYPE_GRAPHICS) {
			listDesc = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		}
		else if (pDesc->mType == COMMAND_TYPE_GRAPHICS) {
			listDesc = D3D12_COMMAND_LIST_TYPE_COPY;
		}
		pDevice->pDxDevice->CreateCommandList1(0, listDesc, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pCmd->pDxCmdList));
		pDesc->pPool->pDxCmdAlloc->QueryInterface(&pCmd->pDxCmdAlloc);
		*ppCmd = pCmd;
	}
	void RemoveCmd(Cmd* pCmd) {
		assert(pCmd);
		pCmd->pDxCmdAlloc->Release();
		pCmd->pDxCmdList->Release();
		delete pCmd;
	}

	void CmdReset(Cmd* pCmd) {
		assert(pCmd);
		pCmd->pDxCmdAlloc->Reset();
		pCmd->pDxCmdList->Reset(pCmd->pDxCmdAlloc, nullptr);
	}

	void CmdClose(Cmd* pCmd) {
		assert(pCmd);
		pCmd->pDxCmdList->Close();
	}

	void CmdResourceBarrier(Cmd* pCmd, ID3D12Resource* pRes, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
		assert(pCmd);
		assert(pRes);
		D3D12_RESOURCE_BARRIER barrier{};
		if (before == after && before == D3D12_RESOURCE_STATE_UNORDERED_ACCESS) {
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.UAV.pResource = pRes;
		}
		else {
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = pRes;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = before;
			barrier.Transition.StateAfter = after;
		}
		pCmd->pDxCmdList->ResourceBarrier(1, &barrier);
	}

	void QueueExecute(Queue* pQueue, Cmd* pCmd) {
		assert(pQueue);
		assert(pCmd);
		pQueue->pDxQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&pCmd->pDxCmdList);
	}

	void GetSwapChainBuffer(SwapChain* pSwapChain, uint32_t index, ID3D12Resource** ppRes) {
		assert(pSwapChain);
		assert(ppRes);

		pSwapChain->pDxSwapChain->GetBuffer(index, IID_PPV_ARGS(ppRes));
	}

	void BeginDraw(Device* pDevice, SwapChain* pSwapChain) {
		auto frameIndex = pSwapChain->pDxSwapChain->GetCurrentBackBufferIndex();
		pSwapChain->cmd[frameIndex]->pDxCmdAlloc->Reset();
		pSwapChain->cmd[frameIndex]->pDxCmdList->Reset(pSwapChain->cmd[frameIndex]->pDxCmdAlloc, nullptr);
		ID3D12Resource* _res = nullptr;
		pSwapChain->pDxSwapChain->GetBuffer(frameIndex, IID_PPV_ARGS(&_res));
		CmdResourceBarrier(pSwapChain->cmd[frameIndex], _res, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		_res->Release();
	}
	void EndDraw(Device* pDevice, SwapChain* pSwapChain) {
		auto frameIndex = pSwapChain->pDxSwapChain->GetCurrentBackBufferIndex();
		ID3D12Resource* _res = nullptr;
		pSwapChain->pDxSwapChain->GetBuffer(frameIndex, IID_PPV_ARGS(&_res));
		CmdResourceBarrier(pSwapChain->cmd[frameIndex], _res, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		pSwapChain->cmd[frameIndex]->pDxCmdList->Close();
		QueueExecute(pDevice->pQueue, pSwapChain->cmd[frameIndex]);
		_res->Release();
	}

	void CreateTexture(Device* pDevice, TextureDesc* pDesc, Texture** ppTexture) {
		assert(pDevice);
		assert(pDesc);
		assert(ppTexture);
		Texture* pTexture = new Texture;
		D3D12_RESOURCE_DESC resourceDesc{ D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, pDesc->mWidth, pDesc->mHeight, 1, pDesc->mMipLevels, pDesc->mFormat,{ 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, pDesc->mFlags };
		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		ID3D12Resource* resource;
		pDevice->pResourceAllocator->CreateResource(
		&allocDesc, &resourceDesc,
			pDesc->mState, NULL,
		&pTexture->pAllocation, IID_PPV_ARGS(&resource));
		resource->Release();
		//pTexture->pDxSrv = nullptr;
		*ppTexture = pTexture;
	}

	void CreateTextureFromFile(Device* pDevice, const char* file, Texture** ppTexture) {
		assert(pDevice);
		assert(file);
		assert(ppTexture);
		Texture* pTexture = new Texture;
		std::string file_str = file;
		std::filesystem::path my_path{ file };	//	
		ID3D12Resource* _tex;
		std::unique_ptr<uint8_t[]> picData;
		D3D12_SUBRESOURCE_DATA subresources;
		DirectX::LoadWICTextureFromFile(pDevice->pDxDevice, my_path.wstring().c_str(), &_tex, picData, subresources);
		auto resDesc = _tex->GetDesc();

		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		ID3D12Resource* resource;
		pDevice->pResourceAllocator->CreateResource(
			&allocDesc, &resDesc,
			D3D12_RESOURCE_STATE_COPY_SOURCE, NULL,
			&pTexture->pAllocation, IID_PPV_ARGS(&resource));

		CmdReset(pDevice->pCmd);
		pDevice->pCmd->pDxCmdList->CopyResource(_tex, resource);
		CmdResourceBarrier(pDevice->pCmd, resource, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON);
		CmdClose(pDevice->pCmd);
		QueueExecute(pDevice->pQueue, pDevice->pCmd);
		QueueWait(pDevice->pQueue);
		_tex->Release();
		resource->Release();
		*ppTexture = pTexture;
	}

	//void CreateTextureFromFile(Device* pDevice, const char* file, Texture** ppTexture) {
	//	assert(pDevice);
	//	assert(file);
	//	assert(ppTexture);

	//	Texture* pTexture = new Texture;

	//	std::string file_str = file;
	//	std::filesystem::path my_path{ file };
	//	ID3D12Resource* _tex;

	//	std::unique_ptr<uint8_t[]> ddsData;
	//	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	//	if (file_str.ends_with(".dds")) {
	//		//DirectX::LoadDDSTextureFromFile(pDevice->pDxDevice, my_path.wstring().c_str(), &_tex, ddsData, subresources);
	//	}
	//	else {
	//		subresources.resize(1);
	//		//DirectX::LoadWICTextureFromFile(pDevice->pDxDevice, my_path.wstring().c_str(), &_tex, ddsData, subresources[0]);
	//	}

	//	*ppTexture = pTexture;
	//}

	void RemoveTexture(Texture* pTexture) {
		assert(pTexture);
		pTexture->pAllocation->Release();
		delete pTexture;
	}
}