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
    template <typename T>
    concept Comparable = requires(T x, T y) {
        x > y;
        x < y;

        x >= y;
        x <= y;

        x == y;
        x != y;
    };

    template <typename T>
    concept IsIterator = requires(T t, T t2) {
        ++t;
        t++;
        --t;
        t--;
        *t;
        t == t2;
        t != t2;
        t.operator->();
        t.m_Data;
    };

    template <typename T>
    concept IsCallable = requires(T t) {
        t.operator();
    };

    template <typename T>
    concept IsClass = std::is_class_v<T>;

    template <typename T>
    concept IsVector = requires(T t) {
        t[0];
    };

    template <typename T>
    concept IsMatrix = requires(T t) {
        t[0][0];
    };
} // namespace adh
