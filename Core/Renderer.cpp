//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "Rainbow/Renderer/IRenderer.h"

#pragma warning (disable: 6011)

namespace Rainbow {
	void CreateDevice(Device** ppDevice) {
		if (!ppDevice) {
			return;
		}
		Device* pDevice = new Device;
		D3D12CreateDevice(nullptr, (D3D_FEATURE_LEVEL)0xc200, __uuidof(pDevice->pDxDevice), reinterpret_cast<void**> (&pDevice->pDxDevice));
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