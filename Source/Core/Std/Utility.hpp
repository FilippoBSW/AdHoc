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
#include <iostream>

namespace adh {
    namespace _internal {
        template <typename T>
        struct RemoveReference {
            using Type = T;
        };

        template <typename T>
        struct RemoveReference<T&> {
            using Type = T;
        };

        template <typename T>
        struct RemoveReference<T&&> {
            using Type = T;
        };

        template <typename T>
        struct PointerDefaultDeleter {
            void operator()(T* ptr) noexcept {
                delete ptr;
            }
        };

        template <typename T>
        struct ArrayDefaultDeleter {
            void operator()(T* ptr) noexcept {
                operator delete(ptr);
            }
        };

        template <typename T>
        class Bucket {
          public:
            using Type      = T;
            using BucketPtr = Bucket*;
            using SizeType  = std::size_t;

          public:
            static constexpr SizeType bucketSize{ sizeof(T) > 8u ? 5u : sizeof(T) > 2u ? 10u
                                                                                       : 20u };

          public:
            Type data[bucketSize];
            BucketPtr next;
            SizeType capacity;
        };
    } // namespace _internal

    template <typename T>
    using RemoveReference = typename _internal::RemoveReference<T>::Type;

    template <typename T>
    using ToPureType = std::remove_pointer_t<std::add_pointer_t<std::remove_cv_t<std::remove_pointer_t<std::decay_t<T>>>>>;

    template <typename T>
    [[nodiscard]] constexpr decltype(auto) Move(T&& data) noexcept {
        return static_cast<RemoveReference<T>&&>(data);
    }

    template <typename T>
    [[nodiscard]] constexpr decltype(auto) Forward(RemoveReference<T>& data) noexcept {
        return static_cast<T&&>(data);
    }

    template <typename T>
    [[nodiscard]] constexpr decltype(auto) Forward(RemoveReference<T>&& data) noexcept {
        return static_cast<T&&>(data);
    }

    template <typename T>
    constexpr void Swap(T& lhs, T& rhs) noexcept {
        auto temp{ Move(lhs) };
        lhs = Move(rhs);
        rhs = Move(temp);
    }

    template <typename T>
    struct Lesser {
        using Type = T;
        constexpr bool operator()(Type x, Type y) const noexcept {
            return x < y;
        }
    };

    template <typename T>
    struct Greater {
        using Type = T;
        constexpr bool operator()(Type x, Type y) const noexcept {
            return x > y;
        }
    };

    template <typename R, typename... Args>
    void* VoidCast(R (*f)(Args... args)) {
        union {
            R(*pf)
            (Args...);
            void* p;
        };
        pf = f;
        return p;
    }

    template <typename T, typename R, typename... Args>
    void* VoidCast(R (T::*f)(Args... args)) {
        union {
            R(T::*pf)
            (Args...);
            void* p;
        };
        pf = f;
        return p;
    }

    template <typename T, typename R, typename... Args>
    void* VoidCast(R (T::*f)(Args... args) const) {
        union {
            R(T::*pf)
            (Args...) const;
            void* p;
        };
        pf = f;
        return p;
    }
} // namespace adh
