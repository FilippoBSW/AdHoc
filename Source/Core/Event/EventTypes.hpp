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

#pragma once
#include <iostream>

namespace adh {
    using EventID              = std::uint32_t;
    using EventListenerIndex   = std::uint32_t;
    using EventListenerVersion = std::uint32_t;
    using EventListenerType    = std::uint64_t;
    enum class EventListener : EventListenerType {};

    static constexpr EventListenerIndex EventListenerShift{ 0x00000020 };
    static constexpr EventListenerIndex EventListenerInvalid{ 0xffffffff };
    static constexpr EventListenerType null_listener{ std::numeric_limits<EventListenerType>::max() };
    static constexpr EventListenerType PageMaxSize{ 4096u / sizeof(EventListenerType) };

    struct BaseEvent {
        bool isHandled{ false };
    };

    struct WindowEvent : BaseEvent {
        enum class Type : char {
            ePrepared,
            eResized,
            eMinimized,
            eDrop,
            eKillfocus
        };

        WindowEvent(Type type) : type{ type } {
        }

        Type type;
    };

    struct WindowDropEvent : WindowEvent {
        WindowDropEvent(Type type, const char* filePath) : WindowEvent{ type },
                                                           filePath{ filePath } {
        }

        const char* filePath;
    };

    struct SwapchainEvent : BaseEvent {
        enum class Type : char {
            eRecreated
        };

        SwapchainEvent(Type type, void* imageViews, std::uint32_t width, std::uint32_t height) : type{ type },
                                                                                                 imageViews{ imageViews },
                                                                                                 width{ width },
                                                                                                 height{ height } {
        }

        Type type;
        void* imageViews;
        std::uint32_t width;
        std::uint32_t height;
    };

    struct KeyboardEvent : BaseEvent {
        enum class Type : char {
            eInvalid,
            eKeyDown,
            eKeyUp,
            eKeyRepeat
        };

        KeyboardEvent(Type type, std::uint64_t keycode) : type{ type },
                                                          keycode{ keycode } {
        }

        Type type;
        std::uint64_t keycode;
    };

    struct CharEvent : BaseEvent {
        CharEvent(std::uint64_t keycode) : keycode{ keycode } {
        }

        std::uint64_t keycode;
    };

    struct MouseMoveEvent : BaseEvent {
        MouseMoveEvent(std::int16_t x, std::int16_t y) : x{ x },
                                                         y{ y } {
        }

        std::int16_t x;
        std::int16_t y;
    };

    struct MouseButtonEvent : MouseMoveEvent {
        enum class Type : char {
            eInvalid,
            eLeftButtonDown,
            eLeftButtonUp,
            eRightButtonDown,
            eRightButtonUp,
            eMiddleButtonDown,
            eMiddleButtonUp,
        };

        enum class Index : char {
            eLeftButton,
            eRightButton,
            eMiddleButton
        };

        MouseButtonEvent(Index index, Type type, std::int16_t x, std::int16_t y) : index{ index },
                                                                                   type{ type },
                                                                                   MouseMoveEvent{ x, y } {
        }

        Index index;
        Type type;
    };

    struct MouseWheelEvent : MouseMoveEvent {
        MouseWheelEvent(float delta, std::int16_t x, std::int16_t y) : delta{ delta },
                                                                       MouseMoveEvent{ x, y } {
        }

        float delta;
    };

    struct ControllerEvent : BaseEvent {
        enum class Type : char {
            eInvalid,
            eButtonDown,
            eButtonUp,
            eButtonRepeat
        };

        ControllerEvent(Type type, std::uint64_t keycode, std::uint32_t id) : type{ type },
                                                                              keycode{ keycode },
                                                                              id{ id } {
        }

        Type type;
        std::uint64_t keycode;
        std::uint32_t id;
    };

    struct CollisionEvent : BaseEvent {
        enum class Type {
            eCollision,
            eTrigger
        };
        CollisionEvent(Type type, std::uint64_t lhs, std::uint64_t rhs)
            : type{ type },
              entityA{ lhs },
              entityB{ rhs } {
        }
        Type type;
        std::uint64_t entityA;
        std::uint64_t entityB;
    };

    struct CameraEvent : BaseEvent {
        enum class Type : char {
            eHasUpdate
        };

        CameraEvent(Type type) : type{ type } {
        }
        Type type;
    };

    struct StatusEvent : BaseEvent {
        enum class Type {
            eRun,
            eStop,
            ePause,
            eUnpause
        };

        StatusEvent(Type type) : type{ type } {
        }

        Type type;
    };

    struct EditorLogEvent : BaseEvent {
        enum class Type {
            eLog,
            eError
        };

        EditorLogEvent(Type type, const char* message)
            : type{ type },
              message{ message } {
        }

        Type type;
        const char* message;
    };
} // namespace adh
