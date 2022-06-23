//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "../ThirdParty/D3D12MemoryAllocator/include/D3D12MemAlloc.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include <cstdint>

namespace Rainbow {
	enum PipelineType
	{
		PIPELINE_TYPE_GRAPHICS = 0,
		PIPELINE_TYPE_COMPUTE,
		PIPELINE_TYPE_COUNT,
	};

	struct Device {
		IDXGIAdapter4* pDxActiveGPU;
		ID3D12Device* pDxDevice;
		D3D12MA::Allocator* pResourceAllocator;
	};

	void CreateDevice(Device** ppDevice);
	void RemoveDevice(Device* pDevice);

	struct Texture {
		ID3D12Resource* pDxResource;
		D3D12MA::Allocation* pDxAllocation;
	};

	struct TextureDesc {
		DXGI_FORMAT Format;
		uint64_t Width;
		uint32_t Height;
	};

	void CreateTexture(Device* pDevice, TextureDesc* pDesc, Texture** ppTexture);
	void RemoveTexture(Texture* pTexture);
}