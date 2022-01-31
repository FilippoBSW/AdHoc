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

#include "Mouse.hpp"

namespace adh {
    Mouse::Mouse() noexcept {
        Flush();
    }

    void Mouse::SetPosition(MouseMoveEvent* event) noexcept {
        m_Pos.x = event->x;
        m_Pos.y = event->y;
    }

    void Mouse::SetButtonState(MouseButtonEvent* event) noexcept {
        SetPosition(event);
        m_ButtonStates[static_cast<std::int32_t>(event->index)] = event->type;
    }

    void Mouse::SetWheelDelta(MouseWheelEvent* event) noexcept {
        SetPosition(event);
        m_WheelDelta = event->delta;
    }

    Mouse::Position Mouse::GetPosition() const noexcept {
        return m_Pos;
    }

    Mouse::State Mouse::GetButtonState(std::uint32_t index) const noexcept {
        return m_ButtonStates[index];
    }

    std::int32_t Mouse::GetWheelDelta() const noexcept {
        return m_WheelDelta;
    }

    void Mouse::OnUpdate() noexcept {
        Flush();
    }

    void Mouse::OnKillFocus() noexcept {
        Flush();
    }

    void Mouse::Flush() noexcept {
        m_ButtonStates.Fill(State::eInvalid);
        m_WheelDelta = 0;
    }
} // namespace adh
