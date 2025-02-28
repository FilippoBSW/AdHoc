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
