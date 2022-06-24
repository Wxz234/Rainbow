//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "Rainbow/Renderer/IRenderer.h"
#include "Rainbow/Math/Math.h"
#include <Windows.h>

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	Rainbow::Device* pDevice = nullptr;
	Rainbow::CreateDevice(&pDevice);
	Rainbow::TextureDesc desc{ {}, DXGI_FORMAT_R8G8B8A8_UNORM, 800, 600, 1, 1, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_FLAG_NONE };
	Rainbow::Texture* pTexture = nullptr;
	Rainbow::CreateTexture(pDevice, &desc, &pTexture);

	Rainbow::RemoveTexture(pTexture);
	Rainbow::RemoveDevice(pDevice);
	return 0;
}