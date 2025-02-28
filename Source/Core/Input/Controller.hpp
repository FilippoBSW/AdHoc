#pragma once
#include <Event/EventTypes.hpp>
#include <Std/StaticArray.hpp>

namespace adh {
    class Controller {
        friend class Input;

      public:
        using State = ControllerEvent::Type;

      private:
        Controller() noexcept;

        void SetButtonState(ControllerEvent* event) noexcept;

        State GetButtonState(std::uint64_t keycode) const noexcept;

        void OnUpdate() noexcept;

        void OnKillFocus() noexcept;

        void Flush() noexcept;

      private:
        StaticArray<State, 56u> m_ButtonStates;
    };
} // namespace adh
