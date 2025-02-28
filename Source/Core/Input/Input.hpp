#pragma once
#include "Controller.hpp"
#include "Keyboard.hpp"
#include "Keycodes.hpp"
#include "Mouse.hpp"
#include <Event/EventTypes.hpp>

namespace adh {
    class Input {
      public:
        static constexpr std::uint32_t numberOfControllers{ 4u };

      public:
        Input();

        ~Input();

        void Initialize();

        bool GetKey(Keycode keycode) noexcept;

        bool RepeatGetKey(Keycode keycode) noexcept;

        bool GetKeyDown(Keycode keycode) noexcept;

        bool GetKeyUp(Keycode keycode) noexcept;

        Mouse::Position GetMousePosition() const noexcept;

        float GetMousePositionX() const noexcept;

        float GetMousePositionY() const noexcept;

        bool GetLeftMouseButtonDown() const noexcept;

        bool GetLeftMouseButtonUp() const noexcept;

        bool GetRightMouseButtonDown() const noexcept;

        bool GetRightMouseButtonUp() const noexcept;

        bool GetMiddleMouseButtonDown() const noexcept;

        bool GetMiddleMouseButtonUp() const noexcept;

        std::int32_t GetMouseWheelDelta() const noexcept;

        bool GetButton(Keycode keycode, std::uint32_t controllerId = 0u) noexcept;

        bool RepeatGetButton(Keycode keycode, std::uint32_t controllerId = 0u) noexcept;

        bool GetButtonDown(Keycode keycode, std::uint32_t controllerId = 0u) noexcept;

        bool GetButtonUp(Keycode keycode, std::uint32_t controllerId = 0u) noexcept;

        void SetControllerVibration(std::uint32_t leftMotorIntensity, std::uint32_t rightMotorIntensity, std::uint32_t controllerId = 0u) noexcept;

        std::uint32_t GetControllerBatteryLevel(std::uint32_t controllerId = 0u) const noexcept;

        void PollEvents() noexcept;

        void OnUpdate() noexcept;

      private:
        void OnKeyboardEvent(KeyboardEvent* keyboardEvent);

        void OnMouseMoveEvent(MouseMoveEvent* mouseEvent);

        void OnMouseButtonEvent(MouseButtonEvent* mouseEvent);

        void OnMouseWheelEvent(MouseWheelEvent* mouseEvent);

        void OnControllerEvent(ControllerEvent* controllerEvent);

        void OnWindowEvent(WindowEvent* event) noexcept;

      private:
        Keyboard m_Keyboard;
        Keyboard m_RepeatKeyboard;
        Mouse m_Mouse;
        Controller m_Controller[numberOfControllers];
        Controller m_RepeatController[numberOfControllers];
        EventListener m_EventListener;
    };
} // namespace adh
