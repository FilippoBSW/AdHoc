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

#include "Input.hpp"
#include <Event/Event.hpp>

// TODO: apple controller events + abstraction with no if def in this file
#if defined(ADH_WINDOWS)
#    include <windows.h>
#    include <xinput.h>
#    pragma comment(lib, "xinput.lib")
#endif

namespace adh {
    Input::Input() {
    }

    Input::~Input() {
        Event::DestroyListener(m_EventListener);
    }

    void Input::Initialize() {
        m_EventListener = Event::CreateListener();
        Event::AddListener<KeyboardEvent>(m_EventListener, &Input::OnKeyboardEvent, this);
        Event::AddListener<MouseMoveEvent>(m_EventListener, &Input::OnMouseMoveEvent, this);
        Event::AddListener<MouseButtonEvent>(m_EventListener, &Input::OnMouseButtonEvent, this);
        Event::AddListener<MouseWheelEvent>(m_EventListener, &Input::OnMouseWheelEvent, this);
        Event::AddListener<ControllerEvent>(m_EventListener, &Input::OnControllerEvent, this);
        Event::AddListener<WindowEvent>(m_EventListener, &Input::OnWindowEvent, this);
    }

    bool Input::GetKey(Keycode keycode) noexcept {
        if (m_Keyboard.GetKeyState(keycode) == KeyboardEvent::Type::eKeyDown ||
            m_Keyboard.GetKeyState(keycode) == KeyboardEvent::Type::eKeyRepeat) {
            return true;
        }

        return false;
    }

    bool Input::RepeatGetKey(Keycode keycode) noexcept {
        if (m_RepeatKeyboard.GetKeyState(keycode) == KeyboardEvent::Type::eKeyDown ||
            m_RepeatKeyboard.GetKeyState(keycode) == KeyboardEvent::Type::eKeyRepeat) {
            return true;
        }

        return false;
    }

    bool Input::GetKeyDown(Keycode keycode) noexcept {
        if (m_Keyboard.GetKeyState(keycode) == KeyboardEvent::Type::eKeyDown) {
            return true;
        }

        return false;
    }

    bool Input::GetKeyUp(Keycode keycode) noexcept {
        if (m_Keyboard.GetKeyState(keycode) == KeyboardEvent::Type::eKeyUp) {
            return true;
        }

        return false;
    }

    Mouse::Position Input::GetMousePosition() const noexcept {
        return m_Mouse.GetPosition();
    }

    float Input::GetMousePositionX() const noexcept {
        return m_Mouse.GetPosition().x;
    }

    float Input::GetMousePositionY() const noexcept {
        return m_Mouse.GetPosition().y;
    }

    bool Input::GetLeftMouseButtonDown() const noexcept {
        if (m_Mouse.GetButtonState(0) == MouseButtonEvent::Type::eLeftButtonDown) {
            return true;
        }

        return false;
    }

    bool Input::GetLeftMouseButtonUp() const noexcept {
        if (m_Mouse.GetButtonState(0) == MouseButtonEvent::Type::eLeftButtonUp) {
            return true;
        }

        return false;
    }

    bool Input::GetRightMouseButtonDown() const noexcept {
        if (m_Mouse.GetButtonState(1) == MouseButtonEvent::Type::eRightButtonDown) {
            return true;
        }

        return false;
    }

    bool Input::GetRightMouseButtonUp() const noexcept {
        if (m_Mouse.GetButtonState(1) == MouseButtonEvent::Type::eRightButtonUp) {
            return true;
        }

        return false;
    }

    bool Input::GetMiddleMouseButtonDown() const noexcept {
        if (m_Mouse.GetButtonState(2) == MouseButtonEvent::Type::eMiddleButtonDown) {
            return true;
        }

        return false;
    }

    bool Input::GetMiddleMouseButtonUp() const noexcept {
        if (m_Mouse.GetButtonState(2) == MouseButtonEvent::Type::eMiddleButtonUp) {
            return true;
        }

        return false;
    }

    std::int32_t Input::GetMouseWheelDelta() const noexcept {
        return m_Mouse.GetWheelDelta();
    }

    bool Input::GetButton(Keycode keycode, std::uint32_t controllerId) noexcept {
        if (m_Controller[controllerId].GetButtonState(keycode) == ControllerEvent::Type::eButtonDown ||
            m_Controller[controllerId].GetButtonState(keycode) == ControllerEvent::Type::eButtonRepeat) {
            return true;
        }

        return false;
    }

    bool Input::RepeatGetButton(Keycode keycode, std::uint32_t controllerId) noexcept {
        if (m_RepeatController[controllerId].GetButtonState(keycode) == ControllerEvent::Type::eButtonDown ||
            m_RepeatController[controllerId].GetButtonState(keycode) == ControllerEvent::Type::eButtonRepeat) {
            return true;
        }

        return false;
    }

    bool Input::GetButtonDown(Keycode keycode, std::uint32_t controllerId) noexcept {
        if (m_Controller[controllerId].GetButtonState(keycode) == ControllerEvent::Type::eButtonDown) {
            return true;
        }

        return false;
    }

    bool Input::GetButtonUp(Keycode keycode, std::uint32_t controllerId) noexcept {
        if (m_Controller[controllerId].GetButtonState(keycode) == ControllerEvent::Type::eButtonUp) {
            return true;
        }

        return false;
    }

    void Input::SetControllerVibration(std::uint32_t leftMotorIntensity, std::uint32_t rightMotorIntensity, std::uint32_t controllerId) noexcept {
#if defined(ADH_WINDOWS)
        XINPUT_VIBRATION vibration{};
        vibration.wLeftMotorSpeed = leftMotorIntensity * 1000u;
        vibration.wLeftMotorSpeed = rightMotorIntensity * 1000u;
        XInputSetState(controllerId, &vibration);

#endif
    }

    std::uint32_t Input::GetControllerBatteryLevel(std::uint32_t controllerId) const noexcept {
#if defined(ADH_WINDOWS)
        XINPUT_BATTERY_INFORMATION batteryInfo{};
        XInputGetBatteryInformation(controllerId, NULL, &batteryInfo);
        return batteryInfo.BatteryLevel;
#else
        return 0u;
#endif
    }

    void Input::PollEvents() noexcept {
        for (std::uint32_t i{}; i != numberOfControllers; ++i) {
#if defined(ADH_WINDOWS)
            XINPUT_STATE state{};
            XINPUT_KEYSTROKE keystroke{};

            if (XInputGetState(i, &state) == ERROR_SUCCESS) {
                // state.Gamepad.sThumbLX
                // state.Gamepad.sThumbLY
                // state.Gamepad.sThumbRX
                // state.Gamepad.sThumbRY
                if (XInputGetKeystroke(i, 0u, &keystroke) == ERROR_SUCCESS) {
                    switch (keystroke.Flags) {
                    case XINPUT_KEYSTROKE_KEYDOWN:
                        {
                            Event::Dispatch<ControllerEvent>(ControllerEvent::Type::eButtonDown, static_cast<char>(keystroke.VirtualKey), i);
                            break;
                        }
                    case XINPUT_KEYSTROKE_KEYUP:
                        {
                            Event::Dispatch<ControllerEvent>(ControllerEvent::Type::eButtonUp, static_cast<char>(keystroke.VirtualKey), i);
                            break;
                        }
                    case XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_REPEAT:
                        {
                            Event::Dispatch<ControllerEvent>(ControllerEvent::Type::eButtonRepeat, static_cast<char>(keystroke.VirtualKey), i);
                            break;
                        }
                    }
                }
            }
#endif
        }
    }

    void Input::OnUpdate() noexcept {
        m_Keyboard.OnUpdate();
        m_Mouse.OnUpdate();
        for (std::uint32_t i{}; i != numberOfControllers; ++i) {
            m_Controller[i].OnUpdate();
        }
    }

    void Input::OnKeyboardEvent(KeyboardEvent* keyboardEvent) {
        m_Keyboard.SetKeyState(keyboardEvent);
        m_RepeatKeyboard.SetKeyState(keyboardEvent);
        keyboardEvent->isHandled = true;
    }

    void Input::OnMouseMoveEvent(MouseMoveEvent* mouseEvent) {
        m_Mouse.SetPosition(mouseEvent);
        mouseEvent->isHandled = true;
    }

    void Input::OnMouseButtonEvent(MouseButtonEvent* mouseEvent) {
        m_Mouse.SetButtonState(mouseEvent);
        mouseEvent->isHandled = true;
    }

    void Input::OnMouseWheelEvent(MouseWheelEvent* mouseEvent) {
        m_Mouse.SetWheelDelta(mouseEvent);
        mouseEvent->isHandled = true;
    }

    void Input::OnControllerEvent(ControllerEvent* controllerEvent) {
        m_Controller[controllerEvent->id].SetButtonState(controllerEvent);
        m_RepeatController[controllerEvent->id].SetButtonState(controllerEvent);
        controllerEvent->isHandled = true;
    }

    void Input::OnWindowEvent(WindowEvent* event) noexcept {
        if (event->type == WindowEvent::Type::eKillfocus) {
            m_Mouse.OnKillFocus();
            m_Keyboard.OnKillFocus();
            m_RepeatKeyboard.OnKillFocus();
            for (std::uint32_t i{}; i != numberOfControllers; ++i) {
                m_Controller[i].OnKillFocus();
                m_RepeatController[i].OnKillFocus();
            }
        }
    }
} // namespace adh
