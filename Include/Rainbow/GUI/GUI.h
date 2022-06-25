//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "../Renderer/IRenderer.h"

namespace Rainbow {
	struct GUI {
		ID3D12DescriptorHeap* pSrvHeap;
	};

	void CreateGUI(Device* pDevice, SwapChain* pSwapChain, GUI** ppGui);
	void RemoveGUI(GUI* pGui);
}
