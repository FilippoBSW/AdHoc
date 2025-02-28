#include "Controller.hpp"

namespace adh {
    Controller::Controller() noexcept {
        Flush();
    }

    void Controller::SetButtonState(ControllerEvent* event) noexcept {
        m_ButtonStates[event->keycode] = event->type;
    }

    Controller::State Controller::GetButtonState(std::uint64_t keycode) const noexcept {
        return m_ButtonStates[static_cast<char>(keycode)];
    }

    void Controller::OnUpdate() noexcept {
        Flush();
    }

    void Controller::OnKillFocus() noexcept {
        Flush();
    }

    void Controller::Flush() noexcept {
        m_ButtonStates.Fill(State::eInvalid);
    }
} // namespace adh
