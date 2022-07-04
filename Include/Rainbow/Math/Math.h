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
}