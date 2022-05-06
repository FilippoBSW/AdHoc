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
#include <random>

namespace adh {
    class Random {
      public:
        template <typename T = std::int32_t>
        static auto Int(T min, T max) noexcept {
            return GetInstance().IntGenerator(min, max);
        }

        template <typename T = float>
        static auto Real(T min, T max) noexcept {
            return GetInstance().RealGenerator(min, max);
        }

      private:
        Random() noexcept : m_Generator{ std::random_device()() } {
        }

        static Random& GetInstance() noexcept {
            static Random instance{};
            return instance;
        }

        template <typename T>
        auto IntGenerator(T min, T max) noexcept {
            std::uniform_int_distribution<T> distrib{ min, max };
            return distrib(m_Generator);
        }

        template <typename T>
        auto RealGenerator(T min, T max) noexcept {
            std::uniform_real_distribution<T> distrib{ min, max };
            return distrib(m_Generator);
        }

      private:
        std::mt19937_64 m_Generator;
    };
} // namespace adh
