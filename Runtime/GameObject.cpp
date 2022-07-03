//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Runtime/GameObject.h"	

#include <atomic>

namespace Rainbow {
	uint32_t GameObject::GetID() {
		static std::atomic<uint32_t> _id(0);
		if (!ID.has_value()) {
			ID = _id++;
		}
		return ID.value();
	}
}