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
		D3D12_CLEAR_VALUE mClearValue;
		DXGI_FORMAT mFormat;
		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mArraySize;
		uint32_t mMipLevels;
		D3D12_RESOURCE_STATES mStartState;
		D3D12_RESOURCE_FLAGS mFlags;
	};

	void CreateTexture(Device* pDevice, TextureDesc* pDesc, Texture** ppTexture);
	void RemoveTexture(Texture* pTexture);
}