#pragma once
#include <Math/Math.hpp>

namespace adh {
    struct Material {
        Material() = default;
        Material(float roughness, float metallicness, float transparency, int hasTexture, Vector3D albedo)
            : roughness{ roughness },
              metallicness{ metallicness },
              transparency{ transparency },
              hasTexture{ hasTexture },
              albedo{ albedo } {}

        alignas(4) float roughness    = 0.6f;                 // Between 1.0f and 0.0f
        alignas(4) float metallicness = 0.0f;                 // Between 1.0f and 0.0f
        alignas(4) float transparency = 1.0f;                 // Between 1.0f and 0.0f
        alignas(4) int hasTexture     = 0;                    // 1 or 0
        alignas(16) Vector3D albedo   = { 1.0f, 1.0f, 1.0f }; // Between 1.0f and 0.0f
    };
} // namespace adh
