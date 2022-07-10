//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include <cstdint>
#include <cstddef>

namespace Rainbow {
	struct GameObject {
		GameObject();
		virtual ~GameObject() {}
		virtual bool IsRenderable() { return false; }

		uint32_t GetID() const;
	private:
		uint32_t m_id;
	};
}