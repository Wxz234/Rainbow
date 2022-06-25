//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "Rainbow/GUI/GUI.h"

#include <cassert>

namespace Rainbow {
	void CreateGUI(Device* pDevice, SwapChain* pSwapChain, GUI** ppGui) {
		assert(pDevice);
		assert(pSwapChain);
		assert(ppGui);
		GUI* pGui = new GUI;
		*ppGui = pGui;
	}
	void RemoveGUI(GUI* pGui) {
		assert(pGui);
		delete pGui;
	}
}