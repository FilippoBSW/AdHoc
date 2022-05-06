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
#include <iostream>
using Keycode = std::uint64_t;

#if defined(ADH_WINDOWS)
#    define ADH_KEY_SPACE 0x20
#    define ADH_KEY_KPADD 0x6B
#    define ADH_KEY_KPSUB 0x6D
#    define ADH_SHIFT 0x10
#    define ADH_CONTROL 0x11
#    define ADH_MENU 0x12
#    define ADH_PAUSE 0x13
#    define ADH_CAPITAL 0x14
#    define ADH_CLEAR 0x0C
#    define ADH_RETURN 0x0D
#    define ADH_BACK 0x08
#    define ADH_TAB 0x09
#    define ADH_ESCAPE 0x1B
#    define ADH_SPACE 0x20
#    define ADH_PRIOR 0x21
#    define ADH_NEXT 0x22
#    define ADH_END 0x23
#    define ADH_HOME 0x24
#    define ADH_LEFT 0x25
#    define ADH_UP 0x26
#    define ADH_RIGHT 0x27
#    define ADH_DOWN 0x28
#    define ADH_SELECT 0x29
#    define ADH_PRINT 0x2A
#    define ADH_EXECUTE 0x2B
#    define ADH_SNAPSHOT 0x2C
#    define ADH_INSERT 0x2D
#    define ADH_DELETE 0x2E
#    define ADH_HELP 0x2F
#    define ADH_APPS 0x5D
#    define ADH_SLEEP 0x5F
#    define ADH_NUMPAD0 0x60
#    define ADH_NUMPAD1 0x61
#    define ADH_NUMPAD2 0x62
#    define ADH_NUMPAD3 0x63
#    define ADH_NUMPAD4 0x64
#    define ADH_NUMPAD5 0x65
#    define ADH_NUMPAD6 0x66
#    define ADH_NUMPAD7 0x67
#    define ADH_NUMPAD8 0x68
#    define ADH_NUMPAD9 0x69
#    define ADH_MULTIPLY 0x6A
#    define ADH_ADD 0x6B
#    define ADH_SEPARATOR 0x6C
#    define ADH_SUBTRACT 0x6D
#    define ADH_DECIMAL 0x6E
#    define ADH_DIVIDE 0x6F
#    define ADH_F1 0x70
#    define ADH_F2 0x71
#    define ADH_F3 0x72
#    define ADH_F4 0x73
#    define ADH_F5 0x74
#    define ADH_F6 0x75
#    define ADH_F7 0x76
#    define ADH_F8 0x77
#    define ADH_F9 0x78
#    define ADH_F10 0x79
#    define ADH_F11 0x7A
#    define ADH_F12 0x7B
#    define ADH_F13 0x7C
#    define ADH_F14 0x7D
#    define ADH_F15 0x7E
#    define ADH_F16 0x7F
#    define ADH_F17 0x80
#    define ADH_F18 0x81
#    define ADH_F19 0x82
#    define ADH_F20 0x83
#    define ADH_F21 0x84
#    define ADH_F22 0x85
#    define ADH_F23 0x86
#    define ADH_F24 0x87

#    define ADH_KEY_A 0x41
#    define ADH_KEY_B 0x42
#    define ADH_KEY_C 0x43
#    define ADH_KEY_D 0x44
#    define ADH_KEY_E 0x45
#    define ADH_KEY_F 0x46
#    define ADH_KEY_G 0x47
#    define ADH_KEY_H 0x48
#    define ADH_KEY_I 0x49
#    define ADH_KEY_J 0x4A
#    define ADH_KEY_K 0x4B
#    define ADH_KEY_L 0x4C
#    define ADH_KEY_M 0x4D
#    define ADH_KEY_N 0x4E
#    define ADH_KEY_O 0x4F
#    define ADH_KEY_P 0x50
#    define ADH_KEY_Q 0x51
#    define ADH_KEY_R 0x52
#    define ADH_KEY_S 0x53
#    define ADH_KEY_T 0x54
#    define ADH_KEY_U 0x55
#    define ADH_KEY_V 0x56
#    define ADH_KEY_W 0x57
#    define ADH_KEY_X 0x58
#    define ADH_KEY_Y 0x59
#    define ADH_KEY_Z 0x5A

#    define ADH_KEY_0 0x30
#    define ADH_KEY_1 0x31
#    define ADH_KEY_2 0x32
#    define ADH_KEY_3 0x33
#    define ADH_KEY_4 0x34
#    define ADH_KEY_5 0x35
#    define ADH_KEY_6 0x36
#    define ADH_KEY_7 0x37
#    define ADH_KEY_8 0x38
#    define ADH_KEY_9 0x39

// Controller
#    define ADH_BUTTON_A 0x5800
#    define ADH_BUTTON_B 0x5801
#    define ADH_BUTTON_X 0x5802
#    define ADH_BUTTON_Y 0x5803
#    define ADH_BUTTON_RSHOULDER 0x5804
#    define ADH_BUTTON_LSHOULDER 0x5805
#    define ADH_BUTTON_LTRIGGER 0x5806
#    define ADH_BUTTON_RTRIGGER 0x5807

#    define ADH_BUTTON_DPAD_UP 0x5810
#    define ADH_BUTTON_DPAD_DOWN 0x5811
#    define ADH_BUTTON_DPAD_LEFT 0x5812
#    define ADH_BUTTON_DPAD_RIGHT 0x5813
#    define ADH_BUTTON_START 0x5814
#    define ADH_BUTTON_BACK 0x5815
#    define ADH_BUTTON_LTHUMB_PRESS 0x5816
#    define ADH_BUTTON_RTHUMB_PRESS 0x5817

#    define ADH_BUTTON_LTHUMB_UP 0x5820
#    define ADH_BUTTON_LTHUMB_DOWN 0x5821
#    define ADH_BUTTON_LTHUMB_RIGHT 0x5822
#    define ADH_BUTTON_LTHUMB_LEFT 0x5823
#    define ADH_BUTTON_LTHUMB_UPLEFT 0x5824
#    define ADH_BUTTON_LTHUMB_UPRIGHT 0x5825
#    define ADH_BUTTON_LTHUMB_DOWNRIGHT 0x5826
#    define ADH_BUTTON_LTHUMB_DOWNLEFT 0x5827

#    define ADH_BUTTON_RTHUMB_UP 0x5830
#    define ADH_BUTTON_RTHUMB_DOWN 0x5831
#    define ADH_BUTTON_RTHUMB_RIGHT 0x5832
#    define ADH_BUTTON_RTHUMB_LEFT 0x5833
#    define ADH_BUTTON_RTHUMB_UPLEFT 0x5834
#    define ADH_BUTTON_RTHUMB_UPRIGHT 0x5835
#    define ADH_BUTTON_RTHUMB_DOWNRIGHT 0x5836
#    define ADH_BUTTON_RTHUMB_DOWNLEFT 0x5837

#elif defined(ADH_APPLE)
#    include "AppleKeycodes.hpp"
#    define ADH_KEY_A kVK_ANSI_A
#    define ADH_KEY_B kVK_ANSI_B
#    define ADH_KEY_C kVK_ANSI_C
#    define ADH_KEY_D kVK_ANSI_D
#    define ADH_KEY_E kVK_ANSI_E
#    define ADH_KEY_F kVK_ANSI_F
#    define ADH_KEY_G kVK_ANSI_G
#    define ADH_KEY_H kVK_ANSI_H
#    define ADH_KEY_I kVK_ANSI_I
#    define ADH_KEY_J kVK_ANSI_J
#    define ADH_KEY_K kVK_ANSI_K
#    define ADH_KEY_L kVK_ANSI_L
#    define ADH_KEY_M kVK_ANSI_M
#    define ADH_KEY_N kVK_ANSI_N
#    define ADH_KEY_O kVK_ANSI_O
#    define ADH_KEY_P kVK_ANSI_P
#    define ADH_KEY_Q kVK_ANSI_Q
#    define ADH_KEY_R kVK_ANSI_R
#    define ADH_KEY_S kVK_ANSI_S
#    define ADH_KEY_T kVK_ANSI_T
#    define ADH_KEY_U kVK_ANSI_U
#    define ADH_KEY_V kVK_ANSI_V
#    define ADH_KEY_W kVK_ANSI_W
#    define ADH_KEY_X kVK_ANSI_X
#    define ADH_KEY_Y kVK_ANSI_Y
#    define ADH_KEY_Z kVK_ANSI_Z

#    define ADH_KEY_0 0x1D
#    define ADH_KEY_1 0x12
#    define ADH_KEY_2 0x13
#    define ADH_KEY_3 0x14
#    define ADH_KEY_4 0x15
#    define ADH_KEY_5 0x17
#    define ADH_KEY_6 0x16
#    define ADH_KEY_7 0x1A
#    define ADH_KEY_8 0x1C
#    define ADH_KEY_9 0x19

#    define ADH_F1 kVK_F1
#    define ADH_F2 kVK_F2
#    define ADH_F3 kVK_F3
#    define ADH_F4 kVK_F4
#    define ADH_F5 kVK_F5
#    define ADH_F6 kVK_F6
#    define ADH_F7 kVK_F7
#    define ADH_F8 kVK_F8
#    define ADH_F9 kVK_F9
#    define ADH_F10 kVK_F10
#    define ADH_F11 kVK_F11
#    define ADH_F12 kVK_F12

#    define ADH_KEY_SPACE kVK_Space
#    define ADH_SHIFT kVK_Shift
#    define ADH_CONTROL kVK_Command
#    define ADH_RETURN kVK_Return
#    define ADH_TAB kVK_Tab
#    define ADH_ESCAPE kVK_Escape
#    define ADH_SPACE kVK_Space

#    define ADH_LEFT kVK_LeftArrow
#    define ADH_UP kVK_UpArrow
#    define ADH_RIGHT kVK_RightArrow
#    define ADH_DOWN kVK_DownArrow
#    define ADH_DELETE kVK_ForwardDelete
#    define ADH_PRIOR -1
#    define ADH_NEXT -1
#    define ADH_HOME -1
#    define ADH_END -1
#    define ADH_INSERT -1
#    define ADH_BACK kVK_Delete
#    define ADH_NUMPAD0 kVK_ANSI_Keypad0
#    define ADH_NUMPAD1 kVK_ANSI_Keypad1
#    define ADH_NUMPAD2 kVK_ANSI_Keypad2
#    define ADH_NUMPAD3 kVK_ANSI_Keypad3
#    define ADH_NUMPAD4 kVK_ANSI_Keypad4
#    define ADH_NUMPAD5 kVK_ANSI_Keypad5
#    define ADH_NUMPAD6 kVK_ANSI_Keypad6
#    define ADH_NUMPAD7 kVK_ANSI_Keypad7
#    define ADH_NUMPAD8 kVK_ANSI_Keypad8
#    define ADH_NUMPAD9 kVK_ANSI_Keypad9

// TODO: Controller
#    define ADH_BUTTON_A 0x3
#    define ADH_BUTTON_B 0x2
#    define ADH_BUTTON_X 0x0
#    define ADH_BUTTON_Y 0x1
#    define ADH_BUTTON_RSHOULDER 0x8
#    define ADH_BUTTON_LSHOULDER 0x9
#    define ADH_BUTTON_LTRIGGER 0xa
#    define ADH_BUTTON_RTRIGGER 0xb

#    define ADH_BUTTON_DPAD_UP 0x4
#    define ADH_BUTTON_DPAD_DOWN 0x5
#    define ADH_BUTTON_DPAD_LEFT 0x6
#    define ADH_BUTTON_DPAD_RIGHT 0x7

#    define ADH_BUTTON_START 0xc
#    define ADH_BUTTON_BACK 0xd
#    define ADH_BUTTON_LTHUMB_PRESS 0xe
#    define ADH_BUTTON_RTHUMB_PRESS 0xf

#    define ADH_BUTTON_LTHUMB_UP 0x10
#    define ADH_BUTTON_LTHUMB_DOWN 0x11
#    define ADH_BUTTON_LTHUMB_RIGHT 0x12
#    define ADH_BUTTON_LTHUMB_LEFT 0x13

#    define ADH_BUTTON_LTHUMB_UPLEFT 0x14
#    define ADH_BUTTON_LTHUMB_UPRIGHT 0x15
#    define ADH_BUTTON_LTHUMB_DOWNRIGHT 0x16
#    define ADH_BUTTON_LTHUMB_DOWNLEFT 0x17

#    define ADH_BUTTON_RTHUMB_UP 0x18
#    define ADH_BUTTON_RTHUMB_DOWN 0x19
#    define ADH_BUTTON_RTHUMB_RIGHT 0x1a
#    define ADH_BUTTON_RTHUMB_LEFT 0x1b

#    define ADH_BUTTON_RTHUMB_UPLEFT 0x1c
#    define ADH_BUTTON_RTHUMB_UPRIGHT 0x1d
#    define ADH_BUTTON_RTHUMB_DOWNRIGHT 0x1e
#    define ADH_BUTTON_RTHUMB_DOWNLEFT 0x1f
#endif
// TODO: Wayland & X11 Keycodes
