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
		virtual bool IsRenderable() { return false; }

	private:
		Vector3 pos;
		Vector3 dir;
		Vector3 up;
		float FovAngleY;
		float AspectRatio;
		float NearZ;
		float FarZ;
	};
}