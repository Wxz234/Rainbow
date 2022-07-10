//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//
#include "../Include/Rainbow/Runtime/CameraObject.h"	

namespace Rainbow {
	Matrix CameraObject::GetViewMatrix() const {

		return Matrix();
	}
	Matrix CameraObject::GetPerspectiveMatrix() const {
        M.m[0][0] = Width;
        M.m[0][1] = 0.0f;
        M.m[0][2] = 0.0f;
        M.m[0][3] = 0.0f;

        M.m[1][0] = 0.0f;
        M.m[1][1] = Height;
        M.m[1][2] = 0.0f;
        M.m[1][3] = 0.0f;

        M.m[2][0] = 0.0f;
        M.m[2][1] = 0.0f;
        M.m[2][2] = fRange;
        M.m[2][3] = 1.0f;

        M.m[3][0] = 0.0f;
        M.m[3][1] = 0.0f;
        M.m[3][2] = -fRange * NearZ;
        M.m[3][3] = 0.0f;
		return Matrix();
	}
}