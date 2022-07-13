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
#include <cstdint>
#include <string>

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

	void CmdReset(Cmd* pCmd);
	void CmdClose(Cmd* pCmd);
	void CmdResourceBarrier(Cmd* pCmd, ID3D12Resource* pRes, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	struct Queue {
		ID3D12CommandQueue* pDxQueue;
		ID3D12Fence* pDxFence;
		HANDLE pDxWaitIdleFenceEvent;
		uint64_t mFenceValue;

		void* pDeviceRef;
	};

	void QueueExecute(Queue* pQueue, Cmd* pCmd);
	void QueueWait(Queue* pQueue);

	struct Device {
		IDXGIAdapter4* pDxActiveGPU;
		ID3D12Device7* pDxDevice;
		D3D12MA::Allocator* pResourceAllocator;

		Cmd* pCmd;
		Queue* pQueue;

		std::vector<IUnknown*> mAllInterface;
	};

	void CreateDevice(Device** ppDevice);
	void RemoveDevice(Device* pDevice);

	void CreateCmd(Device* pDevice, CommandType mType, Cmd** ppCmd);
	void RemoveCmd(Cmd* pCmd,bool force = false);

	void CreateQueue(Device* pDevice, CommandType mType, Queue** ppQueue);
	void RemoveQueue(Queue* pQueue, bool force = false);

	struct SwapChain {
		IDXGISwapChain4* pDxSwapChain;
		ID3D12DescriptorHeap* pDxRTVHeap;

		uint32_t mImageCount;
		uint32_t mDescriptorSize;

		Cmd** pCmdArray;
		void* pDeviceRef;
	};

	struct SwapChainDesc {
		WindowHandle mWindowHandle;
		uint32_t mImageCount;
		uint32_t mWidth;
		uint32_t mHeight;
		DXGI_FORMAT mColorFormat;
	};

	void CreateSwapChain(Device* pDevice, SwapChainDesc* pDesc, SwapChain** ppSwapChain);
	void RemoveSwapChain(SwapChain* pSwapChain, bool force = false);
	void SwapChainResize(SwapChain* pSwapChain, uint32_t width, uint32_t height, DXGI_FORMAT format);
	void BeginDraw(SwapChain* pSwapChain);
	void EndDraw(SwapChain* pSwapChain);

	enum ShaderStage
	{
		SHADER_STAGE_VERTEX  = 1,
		SHADER_STAGE_PIXEL   = 2,
		SHADER_STAGE_COMPUTE = 3,
		SHADER_STAGE_MESH    = 4,
	};

	struct ShaderDesc {
		ShaderStage mStages;
		const char* pEntryPoint;
	};

	struct Shader {
		ShaderStage mStages;
		void* pByteCode;
		uint32_t mByteCodeSize;
	};

	void CreateShaderFromFile(Device* pDevice, std::string file_path, ShaderDesc* pDesc, Shader** ppShader);
	void CreateShaderFromString(Device* pDevice, std::string shader_string, ShaderDesc* pDesc, Shader** ppShader);
	void RemoveShader(Shader* pShader);
}