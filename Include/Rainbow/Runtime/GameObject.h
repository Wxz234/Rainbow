//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include <cstdint>
#include <optional>

namespace Rainbow {
	struct GameObject {
		virtual ~GameObject() {}
		uint32_t GetID();
		virtual bool IsRenderable() {
			return false;
		}
	protected:
		std::optional<uint32_t> ID;
	};
}