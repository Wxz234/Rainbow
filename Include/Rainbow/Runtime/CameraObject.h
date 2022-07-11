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
		CameraObject(const CameraObject&) = default;
		CameraObject& operator=(const CameraObject&) = default;
		CameraObject(CameraObject&&) = default;
		CameraObject& operator=(CameraObject&&) = default;
		virtual bool IsRenderable() { return false; }
		Matrix GetViewMatrix() const;
		Matrix GetPerspectiveMatrix() const;
	private:
		Vector3 pos{ .0f, .0f, .0f };
		Vector3 dir{ .0f, .0f, 1.f };
		Vector3 up { .0f, 1.f, .0f };
		float FovAngleY;
		float AspectRatio;
		float NearZ;
		float FarZ;
	};
}