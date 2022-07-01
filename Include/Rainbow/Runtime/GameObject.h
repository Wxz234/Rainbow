//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include <cstdint>

namespace Rainbow {
	struct GameObject {
		virtual ~GameObject() {}
		virtual uint32_t GetID() const = 0;
	};
}