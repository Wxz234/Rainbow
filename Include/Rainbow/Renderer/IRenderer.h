//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include <d3d12.h>
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
		ID3D12Device* pDxDevice;
	};

	struct MemoryAllocator {

	};

	void CreateDevice(Device** ppDevice);
	void RemoveDevice(Device* pDevice);

	struct Texture {
		ID3D12Resource* pDxResource;
	};

	struct TextureDesc {
		DXGI_FORMAT Format;
		uint64_t Width;
		uint32_t Height;
	};

	void CreateTexture(Device* pDevice, TextureDesc* pDesc, Texture** ppTexture);
	void RemoveTexture(Texture* pTexture);

	struct Material {
		std::string name;
	};
}