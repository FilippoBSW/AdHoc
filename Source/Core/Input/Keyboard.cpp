// *********************************************************************************
// MIT License
//
// Copyright (c) 2021 Filippo-BSW
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

#include "Keyboard.hpp"

namespace adh {
    Keyboard::Keyboard() noexcept {
        Flush();
    }

    void Keyboard::SetKeyState(KeyboardEvent* event) noexcept {
        m_KeyStates[event->keycode] = event->type;
    }

    Keyboard::State Keyboard::GetKeyState(std::uint64_t keycode) const noexcept {
        return m_KeyStates[keycode];
    }

    void Keyboard::OnUpdate() noexcept {
        Flush();
    }

    void Keyboard::OnKillFocus() noexcept {
        Flush();
    }

    void Keyboard::Flush() noexcept {
        m_KeyStates.Fill(State::eInvalid);
    }
} // namespace adh
