//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//
#include "../Include/Rainbow/Runtime/CameraObject.h"	
#include <DirectXMath.h>
#include <cstring>

namespace Rainbow {

    inline void _ScalarSinCos
    (
        float* pSin,
        float* pCos,
        float  Value
    )
    {
        assert(pSin);
        assert(pCos);

        constexpr float _1DIV2PI = 0.159154943f;
        constexpr float _2PI = 6.283185307f;
        constexpr float _PIDIV2 = 1.570796327f;
        constexpr float _PI = 3.141592654f;

        // Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
        float quotient = _1DIV2PI * Value;
        if (Value >= 0.0f)
        {
            quotient = static_cast<float>(static_cast<int>(quotient + 0.5f));
        }
        else
        {
            quotient = static_cast<float>(static_cast<int>(quotient - 0.5f));
        }
        float y = Value - _2PI * quotient;

        // Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
        float sign;
        if (y > _PIDIV2)
        {
            y = _PI - y;
            sign = -1.0f;
        }
        else if (y < -_PIDIV2)
        {
            y = -_PI - y;
            sign = -1.0f;
        }
        else
        {
            sign = +1.0f;
        }

        float y2 = y * y;

        // 11-degree minimax approximation
        *pSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

        // 10-degree minimax approximation
        float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
        *pCos = sign * p;
    }

	Matrix CameraObject::GetViewMatrix() const {

        Vector3 R2 = Vector3Normalize(dir);

		return Matrix();
	}
	Matrix CameraObject::GetPerspectiveMatrix() const {
        float    SinFov;
        float    CosFov;
        _ScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);
        float Height = CosFov / SinFov;
        float Width = Height / AspectRatio;
        float fRange = FarZ / (FarZ - NearZ);

        Matrix M;
        M.r[0] = Vector4(Width, 0, 0, 0);
        M.r[1] = Vector4(0, Height, 0, 0);
        M.r[2] = Vector4(0, 0, fRange, 1);
        M.r[3] = Vector4(0, 0, -fRange * NearZ, 0);
		return M;
	}
}