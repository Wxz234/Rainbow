//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "../Window/Window.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcommon.h>
#include <cstdint>
#include <handleapi.h>
#include <vector>

namespace D3D12MA {
	class Allocator;
	class Allocation;
}

namespace Rainbow {

	enum CommandType
	{
		COMMAND_TYPE_GRAPHICS = 0,
		COMMAND_TYPE_COPY,
		COMMAND_TYPE_COMPUTE,
	};

	struct CmdPool {
		ID3D12CommandAllocator* pDxCmdAlloc;
	};

	struct CmdPoolDesc {
		CommandType mType;
	};

	struct Cmd {
		ID3D12GraphicsCommandList* pDxCmdList;
		ID3D12CommandAllocator* pDxCmdAlloc;
	};

	struct CmdDesc {
		CommandType mType;
		CmdPool* pPool;
	};

	void CmdReset(Cmd* pCmd);
	void CmdClose(Cmd* pCmd);
	void CmdResourceBarrier(Cmd* pCmd, ID3D12Resource* pRes, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	struct Queue {
		ID3D12CommandQueue* pDxQueue;
		ID3D12Fence* pDxFence;
		HANDLE pDxWaitIdleFenceEvent;
		uint64_t mFenceValue;
		IDXGISwapChain* pSubmitSwapChain;
	};

	struct QueueDesc {
		CommandType mType;
	};

	void QueueExecute(Queue* pQueue, Cmd* pCmd);
	void QueueWait(Queue* pQueue);

	struct Device {
		IDXGIAdapter4* pDxActiveGPU;
		ID3D12Device7* pDxDevice;

		D3D12MA::Allocator* pResourceAllocator;

		Queue* pQueue;
		CmdPool* pPool;
		Cmd* pCmd;
	};

	void CreateDevice(Device** ppDevice);
	void RemoveDevice(Device* pDevice);

	void CreateQueue(Device* pDevice, QueueDesc* pDesc, Queue** ppQueue);
	void RemoveQueue(Queue* pQueue);

	void CreateCmdPool(Device* pDevice, CmdPoolDesc* pDesc, CmdPool** ppCmdPool);
	void RemoveCmdPool(CmdPool* pCmdPool);

	void CreateCmd(Device* pDevice, CmdDesc* pDesc, Cmd** ppCmd);
	void RemoveCmd(Cmd* pCmd);

	struct SwapChain {
		IDXGISwapChain4* pDxSwapChain;
		ID3D12DescriptorHeap* pDxRTVHeap;
		uint64_t* pFenceValue;
		uint32_t mDescriptorSize;
		Device* pDevice;
		std::vector<CmdPool*> cmdPool;
		std::vector<Cmd*> cmd;
	};

	struct SwapChainDesc {
		WindowHandle mWindowHandle;
		uint32_t mImageCount;
		uint32_t mWidth;
		uint32_t mHeight;
		DXGI_FORMAT mColorFormat;
	};

	void CreateSwapChain(Device* pDevice, SwapChainDesc* pDesc, SwapChain** ppSwapChain);
	void RemoveSwapChain(SwapChain* pSwapChain);

	void SwapChainResize(SwapChain* pSwapChain, uint32_t width, uint32_t height, DXGI_FORMAT format);
	Cmd* GetSwapChainActiveList(SwapChain* pSwapChain);
	D3D12_CPU_DESCRIPTOR_HANDLE GetSwapChainRTV(SwapChain* pSwapChain);
	void SwapChainPresent(SwapChain* pSwapChain);
	
	void BeginDraw(Device* pDevice, SwapChain* pSwapChain);
	void EndDraw(Device* pDevice, SwapChain* pSwapChain);
}