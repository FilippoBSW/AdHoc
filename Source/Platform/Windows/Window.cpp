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

#include <Window.hpp>

namespace adh {
    Window::Window() noexcept : m_WindowHandle{},
                                m_Name{},
                                m_IsOpen{},
                                m_IsMinimized{},
                                m_IsResizing{},
                                m_IsInFocus{},
                                m_IsPrepared{},
                                m_Width{},
                                m_Height{} {
    }

    Window::Window(const char* name, std::int32_t width, std::int32_t height, bool isPrepared, bool setFullscreen) {
        Create(name, width, height, isPrepared, setFullscreen);
    }

    Window::~Window() {
        Clear();
    }

    // TODO: Fullscreen
    void Window::Create(const char* name, std::int32_t width, std::int32_t height, bool isPrepared, bool setFullscreen) {
        RECT rect;
        SystemParametersInfoA(SPI_GETWORKAREA, NULL, &rect, NULL);
        m_Width  = rect.right;
        m_Height = rect.bottom;

        std::int32_t size{ MultiByteToWideChar(CP_UTF8, 0, name, -1, nullptr, 0) };
        m_Name = new wchar_t[size];
        MultiByteToWideChar(CP_UTF8, 0, name, -1, m_Name, size);

        WNDCLASSEX windowClass{};
        windowClass.cbSize        = sizeof(WNDCLASSEX);
        windowClass.style         = CS_CLASSDC | CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc   = WindowProcedure;
        windowClass.cbClsExtra    = 0;
        windowClass.cbWndExtra    = 0;
        windowClass.hInstance     = s_InstanceHandle;
        windowClass.hIcon         = LoadIcon(s_InstanceHandle, IDI_APPLICATION);
        windowClass.hCursor       = LoadCursor(s_InstanceHandle, IDC_ARROW);
        windowClass.hbrBackground = nullptr;
        windowClass.lpszMenuName  = nullptr;
        windowClass.lpszClassName = m_Name;
        windowClass.hIconSm       = LoadIcon(windowClass.hInstance, IDI_APPLICATION);

        RegisterClassEx(&windowClass);

        RECT wr;
        wr.left   = 100;
        wr.right  = width + wr.left;
        wr.top    = 100;
        wr.bottom = height + wr.top;
        AdjustWindowRectEx(&wr, WS_CAPTION | WS_MINIMIZE | WS_SYSMENU, FALSE, NULL);

        m_WindowHandle = CreateWindowEx(
            WS_EX_CONTROLPARENT,
            m_Name,
            m_Name,
            WS_CAPTION | WS_MINIMIZE | WS_SYSMENU | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_TILEDWINDOW | WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            wr.right - wr.left,
            wr.bottom - wr.top,
            nullptr,
            nullptr,
            s_InstanceHandle,
            this);

        ShowWindow(m_WindowHandle, SW_MAXIMIZE);
        UpdateWindow(m_WindowHandle);
        DragAcceptFiles(m_WindowHandle, TRUE);

        m_IsOpen      = true;
        m_IsMinimized = false;
        m_IsResizing  = false;
        m_IsPrepared  = isPrepared;
    }

    void Window::Destroy() noexcept {
        Clear();
    }

    HWND Window::GetHandle() const noexcept {
        return m_WindowHandle;
    }

    HINSTANCE Window::GetInstance() noexcept {
        return s_InstanceHandle;
    }

    void Window::PollEvents() noexcept {
        MSG message{};

        if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                m_IsOpen = false;
            }

            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    bool Window::IsOpen() const noexcept {
        return m_IsOpen;
    }

    bool Window::IsMinimized() const noexcept {
        return m_IsMinimized;
    }

    bool Window::IsResizing() const noexcept {
        return m_IsResizing;
    }

    bool Window::IsInFocus() const noexcept {
        return m_IsInFocus;
    }

    bool Window::IsMouseInFrame() const noexcept {
        return m_IsMouseInFrame;
    }

    void Window::SetOpen(bool isOpen) noexcept {
        m_IsOpen = isOpen;
    }

    std::int32_t Window::GetScreenWidth() const noexcept {
        return GetSystemMetrics(SM_CXSCREEN);
    }

    std::int32_t Window::GetScreenHeight() const noexcept {
        return GetSystemMetrics(SM_CYSCREEN);
    }

    float Window::GetScreenAspectRatio() const noexcept {
        return static_cast<float>(GetSystemMetrics(SM_CXSCREEN)) / static_cast<float>(GetSystemMetrics(SM_CYSCREEN));
    }

    std::int32_t Window::GetWindowWidth() const noexcept {
        return m_Width;
    }

    std::int32_t Window::GetWindowHeight() const noexcept {
        return m_Height;
    }

    LRESULT WINAPI Window::WindowProcedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) noexcept {
        Window* pThis;

        if (message == WM_NCCREATE) {
            pThis = static_cast<Window*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
            SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        } else {
            pThis = reinterpret_cast<Window*>(GetWindowLongPtr(handle, GWLP_USERDATA));
        }

        switch (message) {
        case WM_DROPFILES:
            {
                HDROP hDropInfo{ reinterpret_cast<HDROP>(wParam) };
                TCHAR buffer[MAX_PATH + 1];

                DragQueryFile(hDropInfo, 0, buffer, MAX_PATH + 1);

                std::int32_t size{ WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, 0, 0) };
                char* name = new char[size];
                WideCharToMultiByte(CP_UTF8, 0, buffer, -1, name, size, 0, 0);

                Event::Dispatch<WindowDropEvent>(WindowEvent::Type::eDrop, name);

                delete[] name;

                DragFinish(hDropInfo);

                break;
            }

        case WM_CLOSE:
        case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            }
        case WM_SETFOCUS:
            {
                pThis->m_IsInFocus = true;
                break;
            }

        case WM_KILLFOCUS:
            {
                pThis->m_IsInFocus = false;
                Event::Dispatch<WindowEvent>(WindowEvent::Type::eKillfocus);
                break;
            }

        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
            {
                if (!(lParam & 0x40'000'000)) {
                    Event::Dispatch<KeyboardEvent>(KeyboardEvent::Type::eKeyDown, wParam);
                } else {
                    Event::Dispatch<KeyboardEvent>(KeyboardEvent::Type::eKeyRepeat, wParam);
                }
                break;
            }

        case WM_KEYUP:
            {
                Event::Dispatch<KeyboardEvent>(KeyboardEvent::Type::eKeyUp, wParam);
                break;
            }

        case WM_CHAR:
            {
                Event::Dispatch<CharEvent>(wParam);
                break;
            }

        case WM_MOUSEMOVE:
            {
                const POINTS points = MAKEPOINTS(lParam);
                Event::Dispatch<MouseMoveEvent>(points.x, points.y);

                if (!pThis->m_IsMouseInFrame) {
                    TRACKMOUSEEVENT tme{};
                    tme.cbSize    = sizeof(tme);
                    tme.dwFlags   = TME_LEAVE;
                    tme.hwndTrack = pThis->m_WindowHandle;
                    TrackMouseEvent(&tme);
                    pThis->m_IsMouseInFrame = true;
                }
                break;
            }

        case WM_LBUTTONDOWN:
            {
                const POINTS points = MAKEPOINTS(lParam);
                Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eLeftButton, MouseButtonEvent::Type::eLeftButtonDown, points.x, points.y);
                break;
            }

        case WM_RBUTTONDOWN:
            {
                const POINTS points = MAKEPOINTS(lParam);
                Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eRightButton, MouseButtonEvent::Type::eRightButtonDown, points.x, points.y);
                break;
            }

        case WM_MBUTTONDOWN:
            {
                const POINTS points = MAKEPOINTS(lParam);
                Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eMiddleButton, MouseButtonEvent::Type::eMiddleButtonDown, points.x, points.y);
                break;
            }

        case WM_LBUTTONUP:
            {
                const POINTS points = MAKEPOINTS(lParam);
                Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eLeftButton, MouseButtonEvent::Type::eLeftButtonUp, points.x, points.y);
                break;
            }

        case WM_RBUTTONUP:
            {
                const POINTS points = MAKEPOINTS(lParam);
                Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eRightButton, MouseButtonEvent::Type::eRightButtonUp, points.x, points.y);
                break;
            }

        case WM_MBUTTONUP:
            {
                const POINTS points = MAKEPOINTS(lParam);
                Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eMiddleButton, MouseButtonEvent::Type::eMiddleButtonUp, points.x, points.y);
                break;
            }

        case WM_MOUSEWHEEL:
            {
                const POINTS points    = MAKEPOINTS(lParam);
                const float wheelDelta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<float>(WHEEL_DELTA);
                Event::Dispatch<MouseWheelEvent>(wheelDelta, points.x, points.y);
                break;
            }

        case WM_MOUSELEAVE:
            {
                pThis->m_IsMouseInFrame = false;
                break;
            }

        case WM_ENTERSIZEMOVE:
            {
                pThis->m_IsResizing = true;
                break;
            }

        case WM_EXITSIZEMOVE:
            {
                pThis->m_IsResizing = false;
                break;
            }

        case WM_SIZE:
            {
                if (wParam == SIZE_MINIMIZED) {
                    pThis->m_IsMinimized = true;
                } else {
                    pThis->m_IsMinimized = false;
                }

                if (!pThis->m_IsMinimized) {
                    if (((wParam == SIZE_MAXIMIZED) || (wParam == SIZE_RESTORED))) {
                        RECT wr;
                        wr.left   = 0;
                        wr.right  = LOWORD(lParam);
                        wr.top    = 0;
                        wr.bottom = HIWORD(lParam);
                        AdjustWindowRectEx(&wr, WS_CAPTION | WS_MINIMIZE | WS_SYSMENU, FALSE, NULL);

                        pThis->m_Width  = wr.right;
                        pThis->m_Height = wr.bottom;

                        Event::Dispatch<WindowEvent>(WindowEvent::Type::eResized);
                    }
                }
                break;
            }
        }

        return DefWindowProc(handle, message, wParam, lParam);
    }

    void Window::Clear() noexcept {
        if (s_InstanceHandle) {
            UnregisterClass(m_Name, s_InstanceHandle);
            s_InstanceHandle = nullptr;
        }

        if (m_WindowHandle) {
            DestroyWindow(m_WindowHandle);
            DragAcceptFiles(m_WindowHandle, FALSE);
            m_WindowHandle = nullptr;
        }

        if (m_Name) {
            delete[] m_Name;
            m_Name = nullptr;
        }
    }
} // namespace adh
