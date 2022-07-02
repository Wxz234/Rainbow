//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Renderer/IRenderer.h"

#include <d3d12sdklayers.h>
#include <combaseapi.h>
#include <wrl/client.h>
#include <winerror.h>
#include <objbase.h>
#include <synchapi.h>
#include <WinBase.h>
#include <cassert>

#pragma warning(disable : 6031)

namespace Rainbow {

	template <typename T>
	void _Erase(void* device, T* p) {
		auto pDevice = (Device*)device;
		std::erase_if(pDevice->mAllInterface,
			[&](IUnknown* ptr)
			{
				IUnknown* temp;
				p->QueryInterface(&temp);
				if (ptr == temp) {
					ptr->Release();
					temp->Release();
					return true;
				}
				temp->Release();
				return false;
			}
		);
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
	void QueueWait(Queue* pQueue) {
		const uint64_t fence = pQueue->mFenceValue++;
		pQueue->pDxQueue->Signal(pQueue->pDxFence, fence);
		if (pQueue->pDxFence->GetCompletedValue() < fence)
		{
			pQueue->pDxFence->SetEventOnCompletion(fence, pQueue->pDxWaitIdleFenceEvent);
			WaitForSingleObject(pQueue->pDxWaitIdleFenceEvent, INFINITE);
		}
	}

	void CreateDevice(Device** ppDevice) {
		assert(ppDevice);

		CoInitialize(NULL);
		Device* pDevice = new Device;
		Microsoft::WRL::ComPtr<IDXGIFactory7> temp_factory;

#ifdef _DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
		CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&temp_factory));
#else
		CreateDXGIFactory2(0, IID_PPV_ARGS(&temp_factory));
#endif // _DEBUG
		temp_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&pDevice->pDxActiveGPU));
		D3D12CreateDevice(pDevice->pDxActiveGPU, (D3D_FEATURE_LEVEL)0xc200, IID_PPV_ARGS(&pDevice->pDxDevice));

		D3D12MA::ALLOCATOR_DESC allocdesc = {};
		allocdesc.Flags = D3D12MA::ALLOCATOR_FLAG_NONE;
		allocdesc.pDevice = pDevice->pDxDevice;
		allocdesc.pAdapter = pDevice->pDxActiveGPU;
		D3D12MA::CreateAllocator(&allocdesc, &pDevice->pResourceAllocator);

		CreateQueue(pDevice, COMMAND_TYPE_GRAPHICS, &pDevice->pQueue);
		CreateCmd(pDevice, COMMAND_TYPE_GRAPHICS, &pDevice->pCmd);

		*ppDevice = pDevice;
	}

	void RemoveDevice(Device* pDevice) {
		assert(pDevice);
		RemoveQueue(pDevice->pQueue);
		RemoveCmd(pDevice->pCmd);

		for (auto& ptr : pDevice->mAllInterface) {
			auto refCount = ptr->AddRef();
			assert(refCount == 2);
			ptr->Release();
			ptr->Release();
		}

		pDevice->pResourceAllocator->Release();
		pDevice->pDxDevice->Release();
		pDevice->pDxActiveGPU->Release();

		delete pDevice;
	}

	void CreateCmd(Device* pDevice, CommandType mType, Cmd** ppCmd) {
		assert(pDevice);
		assert(ppCmd);
		Cmd* pCmd = new Cmd;
		D3D12_COMMAND_LIST_TYPE listDesc{};
		if (mType == COMMAND_TYPE_GRAPHICS) {
			listDesc = D3D12_COMMAND_LIST_TYPE_DIRECT;
		}
		else if (mType == COMMAND_TYPE_COMPUTE) {
			listDesc = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		}
		else if (mType == COMMAND_TYPE_COPY) {
			listDesc = D3D12_COMMAND_LIST_TYPE_COPY;
		}
		pDevice->pDxDevice->CreateCommandAllocator(listDesc, IID_PPV_ARGS(&pCmd->pDxCmdAlloc));
		pDevice->pDxDevice->CreateCommandList1(0, listDesc, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pCmd->pDxCmdList));
		pCmd->pDeviceRef = pDevice;

		IUnknown* _save_list = nullptr;
		IUnknown* _save_alloc = nullptr;
		pCmd->pDxCmdList->QueryInterface(&_save_list);
		pCmd->pDxCmdAlloc->QueryInterface(&_save_alloc);
		pDevice->mAllInterface.push_back(_save_list);
		pDevice->mAllInterface.push_back(_save_alloc);
		*ppCmd = pCmd;
	}
	void RemoveCmd(Cmd* pCmd, bool force) {
		assert(pCmd);

		if (force) {
			_Erase(pCmd->pDeviceRef, pCmd->pDxCmdAlloc);
			_Erase(pCmd->pDeviceRef, pCmd->pDxCmdList);
		}

		pCmd->pDxCmdAlloc->Release();
		pCmd->pDxCmdList->Release();

		delete pCmd;
	}

	void CreateQueue(Device* pDevice, CommandType mType, Queue** ppQueue) {
		assert(pDevice);
		assert(ppQueue);
		Queue* pQueue = new Queue;
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		if (mType == COMMAND_TYPE_GRAPHICS) {
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		}
		else if (mType == COMMAND_TYPE_COMPUTE) {
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		}
		else if (mType == COMMAND_TYPE_COPY) {
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		}

		pDevice->pDxDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pQueue->pDxQueue));
		pDevice->pDxDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pQueue->pDxFence));
		pQueue->pDxWaitIdleFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		pQueue->mFenceValue = 1;
		pQueue->pDeviceRef = pDevice;
		*ppQueue = pQueue;
	}
	void RemoveQueue(Queue* pQueue, bool force) {
		assert(pQueue);
		const uint64_t fence = pQueue->mFenceValue;
		pQueue->pDxQueue->Signal(pQueue->pDxFence, fence);
		if (pQueue->pDxFence->GetCompletedValue() < fence)
		{
			pQueue->pDxFence->SetEventOnCompletion(fence, pQueue->pDxWaitIdleFenceEvent);
			WaitForSingleObject(pQueue->pDxWaitIdleFenceEvent, INFINITE);
		}

		if (force) {
			_Erase(pQueue->pDeviceRef, pQueue->pDxFence);
			_Erase(pQueue->pDeviceRef, pQueue->pDxQueue);
		}

		pQueue->pDxFence->Release();
		pQueue->pDxQueue->Release();
		CloseHandle(pQueue->pDxWaitIdleFenceEvent);

		delete pQueue;
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

		Microsoft::WRL::ComPtr<IDXGIFactory7> temp_factory;
#ifdef _DEBUG
		CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&temp_factory));
#else
		CreateDXGIFactory2(0, IID_PPV_ARGS(&temp_factory));
#endif //_DEBUG
		Microsoft::WRL::ComPtr<IDXGISwapChain1> temp_swapchain;
		//temp_factory->CreateSwapChainForHwnd(pDevice->pQueue->pDxQueue, (HWND)pDesc->mWindowHandle.window, &_desc, &fsSwapChainDesc, nullptr, &temp_swapchain);
		//temp_swapchain->QueryInterface(&pSwapChain->pDxSwapChain);
		//temp_factory->MakeWindowAssociation((HWND)pDesc->mWindowHandle.window, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
	}
	void RemoveSwapChain(SwapChain* pSwapChain) {

	}
}
