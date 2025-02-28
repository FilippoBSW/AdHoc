#pragma once
#include <Event/EventTypes.hpp>
#include <Std/StaticArray.hpp>

namespace adh {
    class Keyboard {
        friend class Input;
        friend class UIOverlay;

      public:
        using State = KeyboardEvent::Type;

      private:
        Keyboard() noexcept;

        void SetKeyState(KeyboardEvent* event) noexcept;

        State GetKeyState(std::uint64_t keycode) const noexcept;

        void OnUpdate() noexcept;

        void OnKillFocus() noexcept;

        void Flush() noexcept;

      private:
        StaticArray<State, 256u> m_KeyStates;
    };
} // namespace adh
