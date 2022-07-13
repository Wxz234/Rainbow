//
// Copyright (c) Wxz.
// Licensed under the MIT License.
//
// https://github.com/Wxz234/Rainbow
//

#pragma once
#ifdef RAINBOW_LIB
#define RAINBOW_EXPORT __declspec(dllexport)
#else
#define RAINBOW_EXPORT __declspec(dllimport)
#endif // RAINBOW_LIB

