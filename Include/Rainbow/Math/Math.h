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

        void Normalize() {
            DirectX::XMFLOAT2 temp_float2(x, y);
            auto temp = DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&temp_float2));
            DirectX::XMStoreFloat2(&temp_float2, temp);
            x = temp_float2.x;
            y = temp_float2.y;
        }

        float x;
        float y;
    };
}