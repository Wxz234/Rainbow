//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../../Include/Rainbow/Window/Window.h"
#include "../../Include/Rainbow/Renderer/IRenderer.h"

#include <Windows.h>
#include <d3d12.h>

uint32_t w = 1024, h = 768;
constexpr uint32_t frameCount = 3;
Rainbow::Window* pWindow = nullptr;
Rainbow::Device* pDevice = nullptr;
Rainbow::SwapChain* pSwapChain = nullptr;
Rainbow::Shader* pVSShader = nullptr;
Rainbow::Shader* pPSShader = nullptr;
Rainbow::RootSignature* pRoot = nullptr;
Rainbow::Pipeline* pPipeline = nullptr;

void Draw() {
	Rainbow::BeginDraw(pSwapChain);
	auto m_commandList = Rainbow::GetActiveCommandList(pSwapChain);
	m_commandList->SetPipelineState(pPipeline->pDxPipelineState);
	m_commandList->SetGraphicsRootSignature(pPipeline->pRootSignature);
	D3D12_VIEWPORT m_viewport(0.0F, 0.0F, (float)w, float(h), D3D12_MIN_DEPTH, D3D12_MAX_DEPTH);
	m_commandList->RSSetViewports(1, &m_viewport);
	D3D12_RECT m_rect{ 0,0,w,h };
	m_commandList->RSSetScissorRects(1, &m_rect);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto rtvHandle = GetActiveRTV(pSwapChain);
	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	m_commandList->DrawInstanced(3, 1, 0, 0);
	Rainbow::EndDraw(pSwapChain);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		if (SIZE_MINIMIZED != wParam) {
			w = LOWORD(lParam);
			h = HIWORD(lParam);
			Rainbow::QueueWait(pDevice->pQueue);
			Rainbow::SwapChainResize(pSwapChain, w, h, DXGI_FORMAT_UNKNOWN);
		}
		break;

	case WM_GETMINMAXINFO:
		if (lParam) {
			auto info = reinterpret_cast<MINMAXINFO*>(lParam);
			info->ptMinTrackSize.x = 320;
			info->ptMinTrackSize.y = 200;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	Rainbow::CreateRenderWindow("RainbowEditor", w, h, WndProc, &pWindow);
	Rainbow::CreateDevice(&pDevice);
	Rainbow::SwapChainDesc swapchainDesc{ pWindow->mWindowHandle, frameCount, w, h, DXGI_FORMAT_R8G8B8A8_UNORM };
	Rainbow::CreateSwapChain(pDevice, &swapchainDesc, &pSwapChain);

	Rainbow::ShaderDesc shaderDesc{ Rainbow::SHADER_STAGE_VERTEX ,"main" };
	Rainbow::CreateShaderFromFile(pDevice, "Shader/02VertexShader.hlsl", &shaderDesc, &pVSShader);
	shaderDesc.mStages = Rainbow::SHADER_STAGE_PIXEL;
	Rainbow::CreateShaderFromFile(pDevice, "Shader/02PixelShader.hlsl", &shaderDesc, &pPSShader);

	Rainbow::CreateRootSignatureFromShader(pDevice, pVSShader, &pRoot);
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { 0,0 };
	psoDesc.pRootSignature = pRoot->pRootSignature;
	psoDesc.VS = { pVSShader->pBlob->GetBufferPointer(), pVSShader->pBlob->GetBufferSize() };
	psoDesc.PS = { pPSShader->pBlob->GetBufferPointer(), pPSShader->pBlob->GetBufferSize() };
	psoDesc.RasterizerState = Rainbow::GetDefaultRasterizerState();
	psoDesc.BlendState = Rainbow::GetDefaultBlendState();
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	Rainbow::CreatePipeline(pDevice, &psoDesc, &pPipeline);

	Rainbow::RenderWindowShow(pWindow);
	Rainbow::RenderWindowRunLoop(pWindow, Draw);

	Rainbow::RemovePipeline(pPipeline, true);
	Rainbow::RemoveRootSignature(pRoot, true);
	Rainbow::RemoveShader(pVSShader,true);
	Rainbow::RemoveShader(pPSShader,true);
	Rainbow::RemoveSwapChain(pSwapChain, true);
	Rainbow::RemoveDevice(pDevice);
	Rainbow::RemoveRenderWindow(pWindow);
	return 0;
}
