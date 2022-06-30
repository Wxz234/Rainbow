//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Window/Window.h"
#include "../Include/Rainbow/Renderer/IRenderer.h"

#include "../ThirdParty/DirectXTex/WICTextureLoader/WICTextureLoader12.h"
#include "../ThirdParty/DirectXTex/WICTextureLoader/d3dx12.h"
#include <Windows.h>
#include <d3dcompiler.h>
#include <string>

uint32_t w = 800, h = 600;
constexpr uint32_t frameCount = 3;
Rainbow::Device* pDevice = nullptr;
Rainbow::SwapChain* pSwapChain = nullptr;
ID3D12PipelineState* pipeline = nullptr;
ID3D12RootSignature* root_signature = nullptr;
ID3D12Resource* pTex;
ID3D12DescriptorHeap* pSrv;
void CreateDefaultPipeline(ID3D12Device* pDevice, ID3D12PipelineState** ppPipeline) {

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	std::string vs_shader = R"(
	#define MyRS1 "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),StaticSampler(s0),DescriptorTable(SRV(t0)) "
	struct PS_INPUT {
		float4 position : SV_POSITION;
		float2 texcoord : TEXCOORD;
	};
	[RootSignature(MyRS1)]
	PS_INPUT main(uint VertexID : SV_VertexID) {
		PS_INPUT Out;
		Out.texcoord = float2((VertexID << 1) & 2, VertexID & 2);
		Out.position = float4(Out.texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
		return Out;
	}
	)";
	std::string ps_shader = R"(
	struct PS_INPUT {
		float4 position : SV_POSITION;
		float2 texcoord: TEXCOORD;
	};
	Texture2D tex : register(t0);
	SamplerState my_Sampler : register(s0);
	float4 main(PS_INPUT ps_in) : SV_TARGET
	{
		float4 color = tex.Sample(my_Sampler,ps_in.texcoord);
		return color;
	}
	)";

	ID3DBlob* vs_blob = nullptr;
	D3DCompile(vs_shader.c_str(), vs_shader.size(), nullptr, nullptr, nullptr, "main", "vs_5_1", 0, 0, &vs_blob, nullptr);
	ID3DBlob* ps_blob = nullptr;
	D3DCompile(ps_shader.c_str(), ps_shader.size(), nullptr, nullptr, nullptr, "main", "ps_5_1", 0, 0, &ps_blob, nullptr);
	//OutputDebugStringA((const char*)error_blob->GetBufferPointer());
	pDevice->CreateRootSignature(0, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), IID_PPV_ARGS(&root_signature));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.InputLayout = { inputElementDescs, 2 };
	psoDesc.pRootSignature = root_signature;
	psoDesc.VS = { vs_blob->GetBufferPointer(), vs_blob->GetBufferSize() };
	psoDesc.PS = { ps_blob->GetBufferPointer(), ps_blob->GetBufferSize() };
	//psoDesc.RasterizerState. = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
	psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthClipEnable = TRUE;
	psoDesc.RasterizerState.MultisampleEnable = FALSE;
	psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
	psoDesc.RasterizerState.ForcedSampleCount = 0;
	psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
	psoDesc.BlendState.IndependentBlendEnable = FALSE;
	const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
	{
		FALSE,FALSE,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL,
	};
	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		psoDesc.BlendState.RenderTarget[i] = defaultRenderTargetBlendDesc;

	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	auto pp = pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(ppPipeline));


}

bool first = true;

void Draw() {
	Rainbow::BeginDraw(pDevice, pSwapChain);

	auto mainList = Rainbow::GetSwapChainActiveList(pSwapChain);
	if (first) {
		//Rainbow::CmdResourceBarrier(mainList, pTex, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
		first = false;
	}
	auto rtv = Rainbow::GetSwapChainRTV(pSwapChain);
	mainList->pDxCmdList->SetPipelineState(pipeline);
	mainList->pDxCmdList->SetGraphicsRootSignature(root_signature);
	mainList->pDxCmdList->SetDescriptorHeaps(1, &pSrv);
	mainList->pDxCmdList->SetGraphicsRootDescriptorTable(0, pSrv->GetGPUDescriptorHandleForHeapStart());
	mainList->pDxCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D12_VIEWPORT viewport{ 0,0,w,h,0.f,1.f };
	mainList->pDxCmdList->RSSetViewports(1, &viewport);
	RECT rect{ 0,0,w,h };
	mainList->pDxCmdList->RSSetScissorRects(1, &rect);

	
	mainList->pDxCmdList->OMSetRenderTargets(1, &rtv, 0, 0);
	mainList->pDxCmdList->DrawInstanced(3, 1, 0, 0);
	Rainbow::EndDraw(pDevice, pSwapChain);
	Rainbow::SwapChainPresent(pSwapChain);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		w = LOWORD(lParam);
		h = HIWORD(lParam);
		Rainbow::QueueWait(pDevice->pQueue);
		Rainbow::SwapChainResize(pSwapChain, w, h, DXGI_FORMAT_UNKNOWN);
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
	Rainbow::Window* pWindow = nullptr;
	Rainbow::CreateRenderWindow("RainbowEditor", w, h, WndProc, &pWindow);
	Rainbow::CreateDevice(&pDevice);
	Rainbow::SwapChainDesc swapchainDesc{ pWindow->mWindowHandle, frameCount, w, h, DXGI_FORMAT_R8G8B8A8_UNORM };
	Rainbow::CreateSwapChain(pDevice, &swapchainDesc, &pSwapChain);

	CreateDefaultPipeline(pDevice->pDxDevice, &pipeline);
	std::unique_ptr<uint8_t[]> wicData;
	D3D12_SUBRESOURCE_DATA subresource;
	DirectX::LoadWICTextureFromFile(pDevice->pDxDevice, L"C:\\Users\\42937\\Documents\\GitHub\\Rainbow\\Asset\\CornellBox.png", &pTex, wicData, subresource);


	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(pTex, 0, 1);

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

	auto desc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	// Create the GPU upload buffer.
	ID3D12Resource* uploadRes;

	pDevice->pDxDevice->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadRes));
	Rainbow::CmdReset(pDevice->pCmd);
	UpdateSubresources(pDevice->pCmd->pDxCmdList, pTex, uploadRes,
		0, 0, 1, &subresource);

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pTex,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
	pDevice->pCmd->pDxCmdList->ResourceBarrier(1, &barrier);

	Rainbow::CmdClose(pDevice->pCmd);
	Rainbow::QueueExecute(pDevice->pQueue, pDevice->pCmd);
	Rainbow::QueueWait(pDevice->pQueue);

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,1,D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,0 };
	pDevice->pDxDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&pSrv));
	pDevice->pDxDevice->CreateShaderResourceView(pTex, nullptr, pSrv->GetCPUDescriptorHandleForHeapStart());

	Rainbow::RenderWindowShow(pWindow);
	Rainbow::RenderWindowRunLoop(pWindow, Draw);

	Rainbow::RemoveSwapChain(pSwapChain);
	Rainbow::RemoveDevice(pDevice);
	Rainbow::RemoveRenderWindow(pWindow);
	return 0;
}