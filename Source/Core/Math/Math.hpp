#pragma once

#if defined(__arm__) || defined(__aarch64__)
#    include <sse2neon/sse2neon.h>
#elif defined(ADH_WINDOWS)
#    include <intrin.h>
#else
#    include <x86intrin.h>
#endif

#if defined(ADH_API_OPENGL) || defined(ADH_MATH_SET_DEPTH_TO_MINUS_ONE)
#    define ADH_MATH_DEPTH_TO_MINUS_ONE true
#else
#    define ADH_MATH_DEPTH_TO_MINUS_ONE false
#endif

#include "source/Numbers.hpp"

#include <Utility.hpp>

#include <concepts>
#include <iostream>

namespace adh {
    template <std::size_t N, typename T>
    class Vector;
    using Vector2D = Vector<2u, float>;
    using Vector3D = Vector<3u, float>;
    using Vector4D = Vector<4u, float>;

    template <std::size_t N, std::size_t N2, typename T>
    class Matrix;
    using Matrix2D = Matrix<2u, 2u, float>;
    using Matrix3D = Matrix<3u, 3u, float>;
    using Matrix4D = Matrix<4u, 4u, float>;

    template <typename T>
    class Quaternion;

    namespace xmm {
        class Vector;
        class Matrix;
    } // namespace xmm
} // namespace adh

#include "source/Matrix.hpp"
#include "source/Matrix2D.hpp"
#include "source/Matrix3D.hpp"
#include "source/Matrix4D.hpp"
#include "source/Quaternion.hpp"
#include "source/Vector.hpp"
#include "source/Vector2D.hpp"
#include "source/Vector3D.hpp"
#include "source/Vector4D.hpp"
#include "source/XmmMatrix.hpp"
#include "source/XmmVector.hpp"

#include "source/Matrix.inl"
#include "source/Matrix2D.inl"
#include "source/Matrix3D.inl"
#include "source/Matrix4D.inl"
#include "source/Quaternion.inl"
#include "source/Vector.inl"
#include "source/Vector2D.inl"
#include "source/Vector3D.inl"
#include "source/Vector4D.inl"
#include "source/XmmMatrix.inl"
#include "source/XmmVector.inl"
