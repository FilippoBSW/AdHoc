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

#pragma once

#if defined(ADH_WINDOWS)
#    include <Windows.h>
#elif defined(ADH_LINUX)
#    include <X11/Xlib.h>
#endif

#include <iostream>

#include <Event/Event.hpp>

namespace adh {
    class Window {
      public:
        Window() noexcept;

        Window(const char* name, std::int32_t width, std::int32_t height, bool isPrepared, bool setFullscreen);

        Window(const Window&) = delete;

        Window& operator=(const Window&) = delete;

        Window(Window&&) = delete;

        Window& operator=(Window&&) = delete;

        ~Window();

        void Create(const char* name, std::int32_t width, std::int32_t height, bool isPrepared, bool setFullscreen);

        void Destroy() noexcept;

        void PollEvents() noexcept;

        bool IsOpen() const noexcept;

        bool IsMinimized() const noexcept;

        bool IsResizing() const noexcept;

        bool IsInFocus() const noexcept;

        bool IsMouseInFrame() const noexcept;

        void SetOpen(bool isOpen) noexcept;

        std::int32_t GetScreenWidth() const noexcept;

        std::int32_t GetScreenHeight() const noexcept;

        float GetScreenAspectRatio() const noexcept;

        std::int32_t GetWindowWidth() const noexcept;

        std::int32_t GetWindowHeight() const noexcept;

      public:
        void Clear() noexcept;

      public:
        bool m_IsOpen;
        bool m_IsMinimized;
        bool m_IsResizing;
        bool m_IsInFocus;
        bool m_IsPrepared;
        bool m_IsMouseInFrame;
        std::int32_t m_Width;
        std::int32_t m_Height;
        std::int32_t m_ScreenWidth;
        std::int32_t m_ScreenHeight;

#if defined(ADH_WINDOWS)
      public:
        HWND GetHandle() const noexcept;

        static HINSTANCE GetInstance() noexcept;

      public:
        static LRESULT WINAPI WindowProcedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) noexcept;

      public:
        HWND m_WindowHandle;
        wchar_t* m_Name;
        inline static HINSTANCE s_InstanceHandle;

#elif defined(ADH_APPLE)
      public:
        void* GetHandle() const noexcept;

      public:
        void* m_WindowHandle;
        const char* m_Name;

#elif defined(ADH_LINUX)
      public:
        Display* GetDisplay() const noexcept;
        ::Window GetWindow() const noexcept;

      private:
        Display* mDisplay;
        ::Window mWindow;
        std::int32_t mScreen;
        const char* m_Name;
#endif
    };
} // namespace adh
