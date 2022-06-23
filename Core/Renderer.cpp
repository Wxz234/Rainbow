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

#pragma warning (disable: 6011)

namespace Rainbow {
	void CreateDevice(Device** ppDevice) {
		if (!ppDevice) {
			return;
		}
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
		if (!pDevice) {
			return;
		}
		pDevice->pDxDevice->Release();
		delete pDevice;
	}
}