//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include <d3d12.h>

namespace Rainbow {
	enum PipelineType
	{
		PIPELINE_TYPE_GRAPHICS = 0,
		PIPELINE_TYPE_COMPUTE,
		PIPELINE_TYPE_COUNT,
	};

	struct Texture {

	};

	struct Device {
		ID3D12Device* pDxDevice;
	};

	void CreateDevice(Device** ppDevice);
	void RemoveDevice(Device* pDevice);
}