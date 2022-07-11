//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once

#include <DirectXMath.h>

namespace Rainbow {
    struct Vector2
    {
        Vector2() { x = y = 0.f; }
        constexpr Vector2(float x, float y) :x(x), y(y) {}
        Vector2(const Vector2&) = default;
        Vector2& operator=(const Vector2&) = default;
        Vector2(Vector2&&) = default;
        Vector2& operator=(Vector2&&) = default;

        float x;
        float y;
    };

    struct Vector3
    {
        Vector3() { x = y = z = 0.f; }
        constexpr Vector3(float x, float y, float z) :x(x), y(y), z(z) {}
        Vector3(const Vector3&) = default;
        Vector3& operator=(const Vector3&) = default;
        Vector3(Vector3&&) = default;
        Vector3& operator=(Vector3&&) = default;

        float x;
        float y;
        float z;
    };

    inline Vector3 GetVector3(DirectX::FXMVECTOR V) {
        Vector3 pDestination;
        DirectX::XMVECTOR T1 = XM_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
        DirectX::XMVECTOR T2 = XM_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
        _mm_store_ss(&pDestination.x, V);
        _mm_store_ss(&pDestination.y, T1);
        _mm_store_ss(&pDestination.z, T2);
        return pDestination;
    }

    inline DirectX::FXMVECTOR GetXMVector3(const Vector3& v) {
        DirectX::XMFLOAT3 temp(v.x, v.y, v.z);
        return DirectX::XMLoadFloat3(&temp);
    }

    struct Vector4
    {
        Vector4() { x = y = z = w = 0.f; }
        constexpr Vector4(float x, float y, float z, float w) :x(x), y(y), z(z), w(w) {}
        Vector4(const Vector4&) = default;
        Vector4& operator=(const Vector4&) = default;
        Vector4(Vector4&&) = default;
        Vector4& operator=(Vector4&&) = default;

        float x;
        float y;
        float z;
        float w;
    };

    struct Matrix {
        Matrix() {}
        Matrix(const Matrix&) = default;
        Matrix& operator=(const Matrix&) = default;
        Matrix(Matrix&&) = default;
        Matrix& operator=(Matrix&&) = default;

        Vector4 r[4]{};
    };

    inline Matrix GetMatrix(DirectX::FXMMATRIX M) {
        Matrix pDestination;
        _mm_storeu_ps((float*)(&pDestination.r[0]), M.r[0]);
        _mm_storeu_ps((float*)(&pDestination.r[1]), M.r[1]);
        _mm_storeu_ps((float*)(&pDestination.r[2]), M.r[2]);
        _mm_storeu_ps((float*)(&pDestination.r[3]), M.r[3]);
        return pDestination;
    }
}