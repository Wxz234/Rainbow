//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#include "../Include/Rainbow/Runtime/GameObject.h"

#include <atomic>

namespace Rainbow {

	GameObject::GameObject() {
		static std::atomic<uint32_t> _id(0);
		m_id = _id++;
	}
	uint32_t GameObject::GetID() const {
		return m_id;
	}
}