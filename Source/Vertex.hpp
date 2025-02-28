#pragma once
#include <Math/Math.hpp>

namespace adh {
    struct Vertex {
        Vertex() = default;
        Vertex(const Vector3D& pos, const Vector3D& n, const Vector2D& t)
            : position{ pos },
              normals{ n },
              textureCoords{ t } {}
        Vector3D position;
        Vector3D normals;
        Vector2D textureCoords;
    };
} // namespace adh
