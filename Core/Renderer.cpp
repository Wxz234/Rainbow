//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Renderer/IRenderer.h"

#include "../ThirdParty/dxc/dxcapi.h"
#include "../ThirdParty/dxc/d3d12shader.h"

#include <d3d12sdklayers.h>
#include <combaseapi.h>
#include <wrl/client.h>
#include <winerror.h>
#include <objbase.h>
#include <synchapi.h>
#include <WinBase.h>
#include <cassert>
#include <vector>
#include <string>
#include <filesystem>

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
	template <typename T>
	void _Save(void* device,T *p) {
		IUnknown* _save = nullptr;
		p->QueryInterface(&_save);
		auto pDevice = (Device*)device;
		pDevice->mAllInterface.push_back(_save);
	}

	namespace _internal {
		IDxcCompiler3* dxcCompiler = nullptr;
		IDxcUtils* dxcUtils = nullptr;
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

		CreateQueue(pDevice, COMMAND_TYPE_GRAPHICS, &pDevice->pQueue);
		CreateCmd(pDevice, COMMAND_TYPE_GRAPHICS, &pDevice->pCmd);

		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&_internal::dxcUtils));
		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&_internal::dxcCompiler));

		*ppDevice = pDevice;
	}

	void RemoveDevice(Device* pDevice) {
		assert(pDevice);
		RemoveQueue(pDevice->pQueue, true);
		RemoveCmd(pDevice->pCmd, false);

		for (auto& ptr : pDevice->mAllInterface) {
			auto refCount = ptr->Release();
			assert(refCount == 0);
		}

		pDevice->pDxDevice->Release();
		pDevice->pDxActiveGPU->Release();

		_internal::dxcUtils->Release();
		_internal::dxcCompiler->Release();

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

		_Save(pCmd->pDeviceRef, pCmd->pDxCmdList);
		_Save(pCmd->pDeviceRef, pCmd->pDxCmdAlloc);
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

		_Save(pQueue->pDeviceRef, pQueue->pDxQueue);
		_Save(pQueue->pDeviceRef, pQueue->pDxFence);

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
		temp_factory->CreateSwapChainForHwnd(pDevice->pQueue->pDxQueue, (HWND)pDesc->mWindowHandle.window, &_desc, &fsSwapChainDesc, nullptr, &temp_swapchain);
		temp_swapchain->QueryInterface(&pSwapChain->pDxSwapChain);
		temp_factory->MakeWindowAssociation((HWND)pDesc->mWindowHandle.window, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
		
		D3D12_DESCRIPTOR_HEAP_DESC heapdesc = {};
		heapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapdesc.NumDescriptors = pDesc->mImageCount;
		heapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		pDevice->pDxDevice->CreateDescriptorHeap(&heapdesc, IID_PPV_ARGS(&pSwapChain->pDxRTVHeap));

		pSwapChain->mDescriptorSize = pDevice->pDxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = pSwapChain->pDxRTVHeap->GetCPUDescriptorHandleForHeapStart();
		for (uint32_t i = 0; i < pDesc->mImageCount; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> pBackBuffer;
			pSwapChain->pDxSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
			pDevice->pDxDevice->CreateRenderTargetView(pBackBuffer.Get(), NULL, rtvHandle);
			rtvHandle.ptr += pSwapChain->mDescriptorSize;
		}

		pSwapChain->mImageCount = pDesc->mImageCount;
	
		auto temp_array = new Cmd * [pSwapChain->mImageCount]{};

		pSwapChain->pCmdArray = temp_array;
		for (uint32_t i = 0; i < pDesc->mImageCount; ++i) {
			CreateCmd(pDevice, COMMAND_TYPE_GRAPHICS, temp_array++);
		}

		pSwapChain->pDeviceRef = pDevice;
		_Save(pSwapChain->pDeviceRef, pSwapChain->pDxSwapChain);
		_Save(pSwapChain->pDeviceRef, pSwapChain->pDxRTVHeap);

		*ppSwapChain = pSwapChain;
	}
	void RemoveSwapChain(SwapChain* pSwapChain, bool force) {
		assert(pSwapChain);
		auto temp_array = pSwapChain->pCmdArray;

		for (uint32_t i = 0; i < pSwapChain->mImageCount; ++i) {
			RemoveCmd(*temp_array++, false);
		}

		if (force) {
			_Erase(pSwapChain->pDeviceRef, pSwapChain->pDxSwapChain);
			_Erase(pSwapChain->pDeviceRef, pSwapChain->pDxRTVHeap);
		}
		pSwapChain->pDxRTVHeap->Release();
		pSwapChain->pDxSwapChain->Release();
		delete[]pSwapChain->pCmdArray;
		delete pSwapChain;
	}

	void SwapChainResize(SwapChain* pSwapChain, uint32_t width, uint32_t height, DXGI_FORMAT format) {
		assert(pSwapChain);
		DXGI_SWAP_CHAIN_DESC scDesc{};
		pSwapChain->pDxSwapChain->GetDesc(&scDesc);
		pSwapChain->pDxSwapChain->ResizeBuffers(scDesc.BufferCount, width, height, format, 0);

		Microsoft::WRL::ComPtr<ID3D12Device> temp_device;
		pSwapChain->pDxRTVHeap->GetDevice(IID_PPV_ARGS(&temp_device));

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = pSwapChain->pDxRTVHeap->GetCPUDescriptorHandleForHeapStart();
		for (uint32_t i = 0; i < scDesc.BufferCount; ++i)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> pBackBuffer;
			pSwapChain->pDxSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
			temp_device->CreateRenderTargetView(pBackBuffer.Get(), NULL, rtvHandle);
			rtvHandle.ptr += pSwapChain->mDescriptorSize;
		}
	}

	void BeginDraw(SwapChain* pSwapChain) {
		assert(pSwapChain);
		auto frameIndex = pSwapChain->pDxSwapChain->GetCurrentBackBufferIndex();
		CmdReset(pSwapChain->pCmdArray[frameIndex]);
		Microsoft::WRL::ComPtr<ID3D12Resource> _res;
		pSwapChain->pDxSwapChain->GetBuffer(frameIndex, IID_PPV_ARGS(&_res));
		CmdResourceBarrier(pSwapChain->pCmdArray[frameIndex], _res.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}
	void EndDraw(SwapChain* pSwapChain) {
		assert(pSwapChain);
		auto frameIndex = pSwapChain->pDxSwapChain->GetCurrentBackBufferIndex();
		Microsoft::WRL::ComPtr<ID3D12Resource> _res;
		pSwapChain->pDxSwapChain->GetBuffer(frameIndex, IID_PPV_ARGS(&_res));
		CmdResourceBarrier(pSwapChain->pCmdArray[frameIndex], _res.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		CmdClose(pSwapChain->pCmdArray[frameIndex]);
		auto pDevice = (Device*)pSwapChain->pDeviceRef;
		QueueExecute(pDevice->pQueue, pSwapChain->pCmdArray[frameIndex]);

		static std::vector<uint64_t> fenceValue(pSwapChain->mImageCount, 0);
		fenceValue[frameIndex] = pDevice->pQueue->mFenceValue++;

		pSwapChain->pDxSwapChain->Present(1, 0);

		const uint64_t currentFenceValue = fenceValue[frameIndex];
		pDevice->pQueue->pDxQueue->Signal(pDevice->pQueue->pDxFence, currentFenceValue);

		frameIndex = pSwapChain->pDxSwapChain->GetCurrentBackBufferIndex();

		if (pDevice->pQueue->pDxFence->GetCompletedValue() < fenceValue[frameIndex])
		{
			pDevice->pQueue->pDxFence->SetEventOnCompletion(fenceValue[frameIndex], pDevice->pQueue->pDxWaitIdleFenceEvent);
			WaitForSingleObjectEx(pDevice->pQueue->pDxWaitIdleFenceEvent, INFINITE, FALSE);
		}
	}

	void CreateShaderFromFile(Device* pDevice, const char* file_path, ShaderDesc* pDesc, Shader** ppShader) {
		assert(pDevice);
		assert(file_path);
		assert(pDesc);
		assert(ppShader);
		
		std::filesystem::path filepath{ file_path };
		auto w_path = filepath.wstring();
		std::filesystem::path entrypoint{ pDesc->pEntryPoint };
		auto w_entrypoint = entrypoint.wstring();

		std::wstring w_target;
		switch (pDesc->mStages)
		{
			case SHADER_STAGE_VERTEX: w_target = L"vs_6_5"; break;
			case SHADER_STAGE_PIXEL: w_target = L"ps_6_5"; break;
			case SHADER_STAGE_COMPUTE: w_target = L"cs_6_5"; break;
			case SHADER_STAGE_MESH: w_target = L"ms_6_5"; break;
		}

		Microsoft::WRL::ComPtr<IDxcBlobEncoding> pSource;
		_internal::dxcUtils->LoadFile(w_path.c_str(), nullptr, &pSource);
		DxcBuffer Source;
		Source.Ptr = pSource->GetBufferPointer();
		Source.Size = pSource->GetBufferSize();
		Source.Encoding = DXC_CP_ACP;

		LPCWSTR pszArgs[] =
		{
			w_path.c_str(),
			L"-E", w_entrypoint.c_str(),
			L"-T", w_target.c_str(),
		};

		Microsoft::WRL::ComPtr<IDxcIncludeHandler> pIncludeHandler;
		_internal::dxcUtils->CreateDefaultIncludeHandler(&pIncludeHandler);

		Microsoft::WRL::ComPtr<IDxcResult> pResults;
		_internal::dxcCompiler->Compile(
			&Source,               
			pszArgs,               
			5,      
			pIncludeHandler.Get(),        
			IID_PPV_ARGS(&pResults)
		);
		
		Shader* pShader = new Shader;
		*ppShader = pShader;
	}

	void RemoveShader(Shader* pShader, bool force) {
	
	}
}
