//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include "../Math/Math.h"

#include "GameObject.h"

namespace Rainbow {
	struct CameraObject : public GameObject {
		CameraObject() {}
		CameraObject(const CameraObject&) = default;
		CameraObject& operator=(const CameraObject&) = default;
		CameraObject(CameraObject&&) = default;
		CameraObject& operator=(CameraObject&&) = default;
		virtual bool IsRenderable() { return false; }
		Matrix GetViewMatrix() const;
		Matrix GetPerspectiveMatrix() const;
		void UpdatePerspective(float FovAngleY, float AspectRatio, float NearZ, float FarZ) {
			this->FovAngleY = FovAngleY;
			this->AspectRatio = AspectRatio;
			this->NearZ = NearZ;
			this->FarZ = FarZ;
		}
	private:
		Vector3 pos{ .0f, .0f, .0f };
		Vector3 dir{ .0f, .0f, 1.f };
		Vector3 up { .0f, 1.f, .0f };
		float FovAngleY = DirectX::XM_PIDIV4;
		float AspectRatio = 1.f;
		float NearZ = 1.f;
		float FarZ = 1000.f;
	};
}