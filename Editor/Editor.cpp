//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Renderer/IRenderer.h"
#include "../Include/Rainbow/GUI/GUI.h"

#include "../ThirdParty/imgui/imgui.h"

#include <Windows.h>
#include <string>

uint32_t width = 1024, height = 768;
constexpr uint32_t frameCount = 3;
Rainbow::Device* pDevice = nullptr;
Rainbow::SwapChain* pSwapChain = nullptr;
Rainbow::Queue* pQueue = nullptr;
Rainbow::GUI* pGui = nullptr;
Rainbow::CmdPool* pCmdPool[frameCount]{};
Rainbow::Cmd* pCmd[frameCount]{};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (Rainbow::GUIWndProcHandler(hWnd, message, wParam, lParam))
		return true;
	switch (message)
	{
	case WM_SIZE:

		width = LOWORD(lParam);
		height = HIWORD(lParam);

		QueueWait(pQueue);
		Rainbow::SwapChainResize(pSwapChain, width, height, DXGI_FORMAT_UNKNOWN);

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void GuiDockSpace() {
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	bool* p_open = nullptr;
	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", p_open, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();

	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows,
			// which we can't undo at the moment without finer window depth/z control.
			bool opt_fullscreen = false;
			ImGui::MenuItem("Test", NULL, &opt_fullscreen);
			ImGui::Separator();

			ImGui::EndMenu();
		}


		ImGui::EndMenuBar();
	}

	ImGui::End();
}

void test() {

}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{	
	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = GetModuleHandle(nullptr);
	wcex.hIcon = LoadIconW(wcex.hInstance, L"IDI_ICON");
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wcex.lpszClassName = L"RainbowEditor";
	wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
	RegisterClassExW(&wcex);

	RECT rc = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	auto stype = WS_OVERLAPPEDWINDOW ;
	AdjustWindowRect(&rc, stype, FALSE);
	auto hwnd = CreateWindowExW(0, L"RainbowEditor", L"RainbowEditor", stype, 10, 10, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

	Rainbow::CreateDevice(&pDevice);
	Rainbow::QueueDesc queueDesc = { Rainbow::COMMAND_TYPE_GRAPHICS };
	Rainbow::CreateQueue(pDevice, &queueDesc, &pQueue);
	Rainbow::SwapChainDesc swapchainDesc{ hwnd, frameCount, width, height, DXGI_FORMAT_R8G8B8A8_UNORM };
	Rainbow::CreateSwapChain(pQueue, &swapchainDesc, &pSwapChain);
	Rainbow::CreateGUI(pDevice, pSwapChain, &pGui);
	//ImGui::LoadIniSettingsFromDisk(const char* ini_filename);
	auto path = RAINBOW_PATH + std::string("Config/editor_imgui.ini");
	ImGui::LoadIniSettingsFromDisk(path.c_str());
	Rainbow::CmdPoolDesc poolDesc{ Rainbow::COMMAND_TYPE_GRAPHICS };
	for (uint32_t i = 0;i < frameCount; ++ i) {
		Rainbow::CreateCmdPool(pDevice, &poolDesc, &pCmdPool[i]);
		Rainbow::CmdDesc cmdDesc{ Rainbow::COMMAND_TYPE_GRAPHICS, pCmdPool[i] };
		Rainbow::CreateCmd(pDevice, &cmdDesc, &pCmd[i]);
	}

	ShowWindow(hwnd, SW_SHOWDEFAULT);
	MSG msg{};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			Rainbow::GUINewFrame(pGui);

			GuiDockSpace();
			ImGui::Begin("Scene");
			ImGui::End();
			ImGui::Begin("Game");
			ImGui::End();
			ImGui::Begin("Content Browser");
			ImGui::End();
			ImGui::Begin("Details");
			ImGui::End();
			
			auto frameIndex = pSwapChain->pDxSwapChain->GetCurrentBackBufferIndex();
			Rainbow::CmdReset(pCmd[frameIndex]);

			ID3D12Resource* res = nullptr;
			Rainbow::GetSwapChainBuffer(pSwapChain, frameIndex, &res);
			Rainbow::CmdResourceBarrier(pCmd[frameIndex], res, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

			auto rtv = Rainbow::GetSwapChainRTV(pSwapChain);
			pCmd[frameIndex]->pDxCmdList->OMSetRenderTargets(1, &rtv, false, nullptr);
			float color[4] = { 0, 0, 0, 1 };
			pCmd[frameIndex]->pDxCmdList->ClearRenderTargetView(rtv, color, 0, nullptr);

			Rainbow::GUIDraw(pGui, pCmd[frameIndex]->pDxCmdList);

			Rainbow::CmdResourceBarrier(pCmd[frameIndex], res, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

			Rainbow::CmdClose(pCmd[frameIndex]);
			Rainbow::QueueExecute(pQueue, pCmd[frameIndex]);

			Rainbow::QueuePresent(pQueue, pSwapChain);

			res->Release();
		}
	}

	for (uint32_t i = 0; i < frameCount; ++i) {
		Rainbow::RemoveCmdPool(pCmdPool[i]);
		Rainbow::RemoveCmd(pCmd[i]);
	}

	Rainbow::RemoveGUI(pGui);
	Rainbow::RemoveSwapChain(pSwapChain);
	Rainbow::RemoveQueue(pQueue);
	Rainbow::RemoveDevice(pDevice);

	return 0;
}