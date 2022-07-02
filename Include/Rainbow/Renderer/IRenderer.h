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
#include <vector>

namespace Rainbow {
	enum CommandType
	{
		COMMAND_TYPE_GRAPHICS = 0,
		COMMAND_TYPE_COPY,
		COMMAND_TYPE_COMPUTE,
	};

	struct Cmd {
		ID3D12GraphicsCommandList* pDxCmdList;
		ID3D12CommandAllocator* pDxCmdAlloc;

		void* pDeviceRef;
	};

	struct Device {
		IDXGIAdapter4* pDxActiveGPU;
		ID3D12Device7* pDxDevice;
		D3D12MA::Allocator* pResourceAllocator;

		Cmd* pCmd;

		std::vector<IUnknown*> mAllInterface;
	};

	void CreateDevice(Device** ppDevice);
	void RemoveDevice(Device* pDevice);

	void CreateCmd(Device* pDevice, CommandType mType, Cmd** ppCmd);
	void RemoveCmd(Cmd* pCmd,bool force = false);

	struct SwapChain {
		IDXGISwapChain4* pDxSwapChain;
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
}