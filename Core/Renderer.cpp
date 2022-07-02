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
#include <cassert>

#pragma warning(disable : 6031)

namespace Rainbow {
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

		*ppDevice = pDevice;
	}

	void RemoveDevice(Device* pDevice) {
		assert(pDevice);

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


}
