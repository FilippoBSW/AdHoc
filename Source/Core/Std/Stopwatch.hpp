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
#include <chrono>
#include <thread>

namespace adh {
    template <typename T = float>
    class Stopwatch {
      public:
        using Type     = T;
        using Clock    = std::chrono::steady_clock;
        using Duration = std::chrono::duration<Type>;

      public:
        Stopwatch() noexcept : m_Start{ Clock::now() },
                               m_End{ Clock::now() } {
        }

        auto GetTime() const noexcept {
            return Duration{ Clock::now() - m_Start }.count();
        }

        auto Lap() noexcept {
            const Type lap{ Duration{ Clock::now() - m_End }.count() };
            m_End = Clock::now();
            return lap;
        }

        auto Reset() noexcept {
            const Clock::time_point temp{ m_Start };
            m_Start = m_End = Clock::now();
            return Duration{ m_Start - temp }.count();
        }

        void SleepFor(std::uint32_t milliseconds) const noexcept {
            std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        }

        void SleepUntil(std::uint32_t milliseconds) const noexcept {
            std::this_thread::sleep_until(m_End + std::chrono::milliseconds(milliseconds));
        }

      private:
        Clock::time_point m_Start;
        Clock::time_point m_End;
    };
} // namespace adh
