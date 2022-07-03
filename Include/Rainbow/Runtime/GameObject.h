//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "Component.h"

#include <cstdint>
#include <cstddef>
#include <optional>

namespace Rainbow {
	struct GameObject {
		virtual ~GameObject() {}
		virtual bool IsRenderable() { return false; }
		virtual Component* GetComponent(size_t i) const = 0;
		virtual size_t GetComponentSize() const = 0;

		uint32_t GetID();
	protected:
		std::optional<uint32_t> ID;
	};
}