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

namespace Rainbow {
	void CreateDevice(Device** ppDevice) {
		assert(ppDevice);

		Device* pDevice = new Device;
#ifdef _DEBUG
		ID3D12Debug* debugController = nullptr;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			debugController->Release();
		}
		IDXGIFactory7* temp_factory = nullptr;
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

	//void CreateTexture(Device* pDevice, TextureDesc* pDesc, Texture** ppTexture) {
	//	assert(pDevice);
	//	assert(pDesc);
	//	assert(ppTexture);

	//	//pDevice->pDxDevice->CreateCommittedResource()
	//}
	//void RemoveTexture(Texture* pTexture) {
	//	assert(pTexture);
	//}
}