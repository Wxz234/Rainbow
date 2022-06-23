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

#endif // DEBUG
		D3D12CreateDevice(nullptr, (D3D_FEATURE_LEVEL)0xc200, IID_PPV_ARGS(&pDevice->pDxDevice));
		*ppDevice = pDevice;
	}

	void RemoveDevice(Device* pDevice) {
		assert(pDevice);

		pDevice->pDxDevice->Release();
		delete pDevice;
	}

	void CreateTexture(Device* pDevice, TextureDesc* pDesc, Texture** ppTexture) {
		assert(pDevice);
		assert(pDesc);
		assert(ppTexture);

		//pDevice->pDxDevice->CreateCommittedResource()
	}
	void RemoveTexture(Texture* pTexture) {
		assert(pTexture);
	}
}