//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "../Core/Core.h"
#include "../Window/Window.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <cstdint>
#include <d3dcommon.h>

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

	RAINBOW_EXPORT void CmdReset(Cmd* pCmd);
	RAINBOW_EXPORT void CmdClose(Cmd* pCmd);
	RAINBOW_EXPORT void CmdResourceBarrier(Cmd* pCmd, ID3D12Resource* pRes, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	struct Queue {
		ID3D12CommandQueue* pDxQueue;
		ID3D12Fence* pDxFence;
		HANDLE pDxWaitIdleFenceEvent;
		uint64_t mFenceValue;

		void* pDeviceRef;
	};

	RAINBOW_EXPORT void QueueExecute(Queue* pQueue, Cmd* pCmd);
	RAINBOW_EXPORT void QueueWait(Queue* pQueue);

	struct Device {
		IDXGIAdapter4* pDxActiveGPU;
		ID3D12Device7* pDxDevice;

		Cmd* pCmd;
		Queue* pQueue;

		std::vector<IUnknown*> mAllInterface;
	};

	RAINBOW_EXPORT void CreateDevice(Device** ppDevice);
	RAINBOW_EXPORT void RemoveDevice(Device* pDevice);

	RAINBOW_EXPORT void CreateCmd(Device* pDevice, CommandType mType, Cmd** ppCmd);
	RAINBOW_EXPORT void RemoveCmd(Cmd* pCmd,bool force = false);

	RAINBOW_EXPORT void CreateQueue(Device* pDevice, CommandType mType, Queue** ppQueue);
	RAINBOW_EXPORT void RemoveQueue(Queue* pQueue, bool force = false);

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

	RAINBOW_EXPORT void CreateSwapChain(Device* pDevice, SwapChainDesc* pDesc, SwapChain** ppSwapChain);
	RAINBOW_EXPORT void RemoveSwapChain(SwapChain* pSwapChain, bool force = false);
	RAINBOW_EXPORT void SwapChainResize(SwapChain* pSwapChain, uint32_t width, uint32_t height, DXGI_FORMAT format);
	RAINBOW_EXPORT void BeginDraw(SwapChain* pSwapChain);
	RAINBOW_EXPORT void EndDraw(SwapChain* pSwapChain);

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
		ID3DBlob* pBlob;
		ID3DBlob* pReflectionData;

		void* pDeviceRef;
	};

	RAINBOW_EXPORT void CreateShaderFromFile(Device* pDevice, const char* file_path, ShaderDesc* pDesc, Shader** ppShader);
	RAINBOW_EXPORT void RemoveShader(Shader* pShader,bool force = false);

	struct Pipeline {
		ID3D12PipelineState* pDxPipelineState;
		ID3D12RootSignature* pDxRoot;
		void* pDeviceRef;
	};
}