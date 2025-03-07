#pragma once

#if defined(ADH_WINDOWS)
#    include <Windows.h>
#elif defined(ADH_LINUX)
#    include <X11/Xlib.h>
#    include <xcb/xcb.h>
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
        xcb_connection_t* GetConnection() const noexcept;
        xcb_window_t GetWindow() const noexcept;

      private:
        Display* m_Display;
        xcb_connection_t* m_Connection;
        xcb_screen_t* m_Screen;
        xcb_window_t m_Window;
        xcb_intern_atom_cookie_t protocols_cookie;
        xcb_intern_atom_reply_t* protocols_reply;
        xcb_intern_atom_cookie_t delete_cookie;
        xcb_intern_atom_reply_t* delete_reply;

        const char* m_Name;
#endif
    };
} // namespace adh
