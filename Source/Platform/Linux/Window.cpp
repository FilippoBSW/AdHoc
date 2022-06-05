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

#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>

#include <array>

namespace adh {
    // static inline xcb_intern_atom_reply_t* intern_atom_helper(xcb_connection_t* conn, bool only_if_exists, const char* str) {
    //     xcb_intern_atom_cookie_t cookie = xcb_intern_atom(conn, only_if_exists, strlen(str), str);
    //     return xcb_intern_atom_reply(conn, cookie, NULL);
    // }

    Window::Window() noexcept : m_Connection{},
                                m_Screen{},
                                m_Window{},
                                // m_Atom_wm_delete_window{},
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
        m_Display        = XOpenDisplay(nullptr);
        m_Connection     = XGetXCBConnection(m_Display);
        auto screenIndex = XDefaultScreen(m_Display);

        const xcb_setup_t* setup   = xcb_get_setup(m_Connection);
        xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
        while (screenIndex-- > 0) {
            xcb_screen_next(&iter);
        }
        m_Screen = iter.data;

        m_Window = xcb_generate_id(m_Connection);

        std::uint32_t mask{ XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK };
        std::uint32_t values[2];
        values[0] = m_Screen->white_pixel;
        values[1] =
            XCB_EVENT_MASK_KEY_RELEASE |
            XCB_EVENT_MASK_KEY_PRESS |
            XCB_EVENT_MASK_EXPOSURE |
            XCB_EVENT_MASK_STRUCTURE_NOTIFY |
            XCB_EVENT_MASK_POINTER_MOTION |
            XCB_EVENT_MASK_BUTTON_PRESS |
            XCB_EVENT_MASK_BUTTON_RELEASE;

        xcb_create_window(m_Connection,
                          XCB_COPY_FROM_PARENT,
                          m_Window,
                          m_Screen->root,
                          0, 0,
                          width, height,
                          0,
                          XCB_WINDOW_CLASS_INPUT_OUTPUT,
                          m_Screen->root_visual,
                          mask,
                          values);

        xcb_flush(m_Connection);

        std::string windowTitle = name;
        xcb_change_property(m_Connection,
                            XCB_PROP_MODE_REPLACE,
                            m_Window,
                            XCB_ATOM_WM_NAME,
                            XCB_ATOM_STRING,
                            8,
                            windowTitle.size(),
                            windowTitle.data());

        m_IsOpen         = true;
        m_IsMinimized    = false;
        m_IsResizing     = false;
        m_IsPrepared     = isPrepared;
        m_IsMouseInFrame = false;
        m_Width          = width;
        m_Height         = height;
        m_ScreenWidth    = width;
        m_ScreenHeight   = height;

        protocols_cookie = xcb_intern_atom(m_Connection, 1, 12, "WM_PROTOCOLS");
        protocols_reply  = xcb_intern_atom_reply(m_Connection, protocols_cookie, 0);
        delete_cookie    = xcb_intern_atom(m_Connection, 0, 16, "WM_DELETE_WINDOW");
        delete_reply     = xcb_intern_atom_reply(m_Connection, delete_cookie, 0);
        xcb_change_property(m_Connection, XCB_PROP_MODE_REPLACE, m_Window, (*protocols_reply).atom, 4, 32, 1, &(*delete_reply).atom);
        free(protocols_reply);

        xcb_map_window(m_Connection, m_Window);
        xcb_flush(m_Connection);

        // xcb_intern_atom_reply_t* reply = intern_atom_helper(m_Connection, true, "WM_PROTOCOLS");
        // m_Atom_wm_delete_window        = intern_atom_helper(m_Connection, false, "WM_DELETE_WINDOW");

        // xcb_change_property(m_Connection, XCB_PROP_MODE_REPLACE,
        //                     m_Window, (*reply).atom, 4, 32, 1,
        //                     &(*m_Atom_wm_delete_window).atom);

        // cookie2 = xcb_intern_atom(m_Connection, 0, 16, "WM_DELETE_WINDOW");
        // reply2  = xcb_intern_atom_reply(m_Connection, cookie2, 0);

        // free(reply);
    }

    void Window::PollEvents() noexcept {
        xcb_generic_event_t* event;

        while((event = xcb_poll_for_event(m_Connection))) {
            std::cout << (event->response_type & 0x08) << std::endl;
            switch (event->response_type & ~0x80) {
            case XCB_CONFIGURE_NOTIFY:
                {
                    std::cout << "Configure Notify" << std::endl;
                    const xcb_configure_notify_event_t* cfgEvent = (const xcb_configure_notify_event_t*)event;
                    if ((m_IsPrepared) && ((cfgEvent->width != m_Width) || (cfgEvent->height != m_Height))) {
                        auto destWidth  = cfgEvent->width;
                        auto destHeight = cfgEvent->height;
                        if ((destWidth > 0) && (destHeight > 0)) {
                            m_ScreenWidth = m_Width = destWidth;
                            m_ScreenHeight = m_Height = destHeight;
                            Event::Dispatch<WindowEvent>(WindowEvent::Type::eResized);
                        }
                    }
                    break;
                }
            case XCB_CLIENT_MESSAGE:
                {
                    std::cout << "Client message" << std::endl;
                    if ((*(xcb_client_message_event_t*)event).data.data32[0] == (*delete_reply).atom) {
                        SetOpen(false);
                        free(delete_reply);
                    }

                    // if ((*(xcb_client_message_event_t*)event).data.data32[0] == (*reply2).atom) {
                    //     std::cout << "Close message!" << std::endl;
                    // }
                    // std::cout << "Client Message" << std::endl;
                    // if ((*(xcb_client_message_event_t*)event).data.data32[0] ==
                    //     (*m_Atom_wm_delete_window).atom) {
                    //     // SetOpen(false);
                    //     std::cout << "Close message" << std::endl;
                    // }
                    break;
                }
            case XCB_KEY_PRESS:
                {
                    std::cout << "Key press" << std::endl;
                    const xcb_key_press_event_t* keyEvent = (const xcb_key_press_event_t*)event;
                    Event::Dispatch<KeyboardEvent>(KeyboardEvent::Type::eKeyDown, keyEvent->detail);

                    XKeyEvent keyev;
                    keyev.display = m_Display;
                    keyev.keycode = keyEvent->detail;
                    keyev.state   = keyEvent->state;

                    char buf;
                    if (XLookupString(&keyev, &buf, 1, nullptr, nullptr)) {
                        Event::Dispatch<CharEvent>((std::uint64_t)buf);
                    }
                    break;
                }
            case XCB_KEY_RELEASE:
                {
                    const xcb_key_release_event_t* keyEvent = (const xcb_key_release_event_t*)event;
                    Event::Dispatch<KeyboardEvent>(KeyboardEvent::Type::eKeyUp, keyEvent->detail);
                    break;
                }
            case XCB_MOTION_NOTIFY:
                {
                    xcb_motion_notify_event_t* motion = (xcb_motion_notify_event_t*)event;
                    Event::Dispatch<MouseMoveEvent>(motion->event_x, motion->event_y);
                    break;
                }
            case XCB_BUTTON_PRESS:
                {
                    xcb_button_press_event_t* press = (xcb_button_press_event_t*)event;
                    switch (press->detail) {
                    case XCB_BUTTON_INDEX_1:
                        {
                            Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eLeftButton, MouseButtonEvent::Type::eLeftButtonDown, press->event_x, press->event_y);
                            break;
                        }
                    case XCB_BUTTON_INDEX_2:
                        {
                            Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eMiddleButton, MouseButtonEvent::Type::eMiddleButtonDown, press->event_x, press->event_y);
                            break;
                        }
                    case XCB_BUTTON_INDEX_3:
                        {
                            Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eRightButton, MouseButtonEvent::Type::eRightButtonDown, press->event_x, press->event_y);
                            break;
                        }
                    }
                    break;
                }
            case XCB_BUTTON_RELEASE:
                {
                    xcb_button_release_event_t* press = (xcb_button_release_event_t*)event;
                    switch (press->detail) {
                    case XCB_BUTTON_INDEX_1:
                        {
                            Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eLeftButton, MouseButtonEvent::Type::eLeftButtonUp, press->event_x, press->event_y);
                            break;
                        }
                    case XCB_BUTTON_INDEX_2:
                        {
                            Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eMiddleButton, MouseButtonEvent::Type::eMiddleButtonUp, press->event_x, press->event_y);
                            break;
                        }
                    case XCB_BUTTON_INDEX_3:
                        {
                            Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eRightButton, MouseButtonEvent::Type::eRightButtonUp, press->event_x, press->event_y);
                            break;
                        }
                    }
                    break;
                }
            }
            free(event);
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

    xcb_connection_t* Window::GetConnection() const noexcept {
        return m_Connection;
    }

    xcb_window_t Window::GetWindow() const noexcept {
        return m_Window;
    }

    void Window::Clear() noexcept {
        xcb_destroy_window(m_Connection, m_Window);
        XCloseDisplay(m_Display);
    }
} // namespace adh
