//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "../Renderer/IRenderer.h"

#include <Windows.h>

namespace Rainbow {
	struct GUI {
		ID3D12DescriptorHeap* pSrvHeap;
	};

	void CreateGUI(Device* pDevice, SwapChain* pSwapChain, GUI** ppGui);
	void RemoveGUI(GUI* pGui);

	void GUINewFrame(GUI* pGui);
	void GUIDraw(GUI* pGui, ID3D12GraphicsCommandList* ctx);
	LRESULT GUIWndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
}
