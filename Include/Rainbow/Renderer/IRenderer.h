//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "../Window/Window.h"

#include <d3d12.h>

namespace Rainbow {
	enum PipelineType
	{
		PIPELINE_TYPE_GRAPHICS = 0,
		PIPELINE_TYPE_COMPUTE,
		PIPELINE_TYPE_COUNT,
	};
}