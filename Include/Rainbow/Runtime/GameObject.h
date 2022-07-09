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

namespace Rainbow {
	struct GameObject {
		GameObject();
		virtual ~GameObject() {}
		virtual bool IsRenderable() { return false; }
		virtual Component* GetComponent(size_t i) const = 0;
		virtual size_t GetComponentSize() const = 0;

		uint32_t GetID() const;
	private:
		uint32_t m_id;
	};
}