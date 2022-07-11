//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//
#include "../Include/Rainbow/Runtime/CameraObject.h"

namespace Rainbow {

	Matrix CameraObject::GetViewMatrix() const {
		auto _pos = GetXMVector3(pos);
		auto _dir = GetXMVector3(dir);
		auto _up = GetXMVector3(up);
		auto m = DirectX::XMMatrixLookToLH(_pos, _dir, _up);
		return GetMatrix(m);
	}
	Matrix CameraObject::GetPerspectiveMatrix() const {
		auto m = DirectX::XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ);
		return GetMatrix(m);
	}
}