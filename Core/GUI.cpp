//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "Rainbow/GUI/GUI.h"

#include "../ThirdParty/imgui/imgui.h"
#include "../ThirdParty/imgui/backends/imgui_impl_dx12.h"

#include <cassert>

namespace Rainbow {
	void CreateGUI(Device* pDevice, SwapChain* pSwapChain, GUI** ppGui) {
		assert(pDevice);
		assert(pSwapChain);
		assert(ppGui);
		GUI* pGui = new GUI;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		*ppGui = pGui;
	}
	void RemoveGUI(GUI* pGui) {
		assert(pGui);
		delete pGui;
	}
}