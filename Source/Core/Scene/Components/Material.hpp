// *********************************************************************************
// MIT License
//
// Copyright (c) 2021-2022 Filippo-BSW
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// *********************************************************************************

#pragma once
#include <Math/Math.hpp>

namespace adh {
    struct Material {
        Material() = default;
        Material(float roughness, float metallicness, float transparency, Vector3D albedo)
            : roughness{ roughness },
              metallicness{ metallicness },
              transparency{ transparency },
              albedo{ albedo } {}

        alignas(4) float roughness    = 0.6f;                 // Between 1.0f and 0.0f
        alignas(4) float metallicness = 0.0f;                 // Between 1.0f and 0.0f
        alignas(4) float transparency = 1.0f;                 // Between 1.0f and 0.0f
        alignas(16) Vector3D albedo   = { 1.0f, 1.0f, 1.0f }; // Between 1.0f and 0.0f
    };
} // namespace adh
