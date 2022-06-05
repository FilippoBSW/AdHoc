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

#include <Window.hpp>

#include <Event/EventTypes.hpp>

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>

#include <array>

namespace adh {
    Window::Window() noexcept : mDisplay{},
                                mScreen{},
                                mWindow{},
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

    void Window::Create(const char* name, std::int32_t width, std::int32_t height, bool isPrepared, bool setFullscreen) {
        mDisplay = XOpenDisplay(nullptr);
        mScreen  = DefaultScreen(mDisplay);
        mWindow  = XCreateSimpleWindow(
             mDisplay, RootWindow(mDisplay, mScreen), 100, 100, width, height, 1,
             BlackPixel(mDisplay, mScreen), WhitePixel(mDisplay, mScreen));

        XSelectInput(mDisplay, mWindow,
                     StructureNotifyMask |
                         KeyPressMask |
                         KeyReleaseMask |
                         PointerMotionMask |
                         ButtonPressMask |
                         ButtonReleaseMask |
                         ExposureMask |
                         FocusChangeMask |
                         VisibilityChangeMask |
                         EnterWindowMask |
                         LeaveWindowMask |
                         PropertyChangeMask);

        m_IsOpen         = true;
        m_IsMinimized    = false;
        m_IsResizing     = false;
        m_IsPrepared     = isPrepared;
        m_IsMouseInFrame = false;
        m_Width          = width;
        m_Height         = height;
        m_ScreenWidth    = width;
        m_ScreenHeight   = height;

        XMapWindow(mDisplay, mWindow);
    }

    void Window::PollEvents() noexcept {
        XEvent event;
        XPeekEvent(mDisplay, &event);

        switch (event.type) {
        case KeyPress:
            {
                char buf;
                if (XLookupString((XKeyEvent*)&event, &buf, 1, nullptr, nullptr)) {
                    std::cout << buf << std::endl;
                }
                break;
            }
        }
    }

    void Window::Destroy() noexcept {
        Clear();
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
        return m_ScreenWidth;
    }

    std::int32_t Window::GetScreenHeight() const noexcept {
        return m_ScreenHeight;
    }

    float Window::GetScreenAspectRatio() const noexcept {
        return (float)m_ScreenWidth / (float)m_ScreenHeight;
    }

    std::int32_t Window::GetWindowWidth() const noexcept {
        return m_Width;
    }

    std::int32_t Window::GetWindowHeight() const noexcept {
        return m_Height;
    }

    Display* Window::GetDisplay() const noexcept {
        return mDisplay;
    }

    ::Window Window::GetWindow() const noexcept {
        return mWindow;
    }

    void Window::Clear() noexcept {
        XDestroyWindow(mDisplay, mWindow);
        XCloseDisplay(mDisplay);
    }

} // namespace adh
