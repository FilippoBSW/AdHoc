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
#include <sstream>

#if defined(ADH_THROW)
#    undef ADH_THROW
#endif
#if defined(ADH_DEBUG)
#    if defined(ADH_WINDOWS)
#        include <Windows.h>
#        define ADH_THROW(expression, message)                                                                            \
            {                                                                                                             \
                if (!(expression)) {                                                                                      \
                    std::wstringstream str;                                                                               \
                    str << "--- Throw! ---"                                                                               \
                        << "\n"                                                                                           \
                        << std::endl;                                                                                     \
                    str << "Message:    " << message << "\n"                                                              \
                        << std::endl;                                                                                     \
                    str << "expressiontion:   "                                                                           \
                        << "[ " << __func__ << " ]" << std::endl;                                                         \
                    str << "Line:       "                                                                                 \
                        << "[ " << __LINE__ << " ]" << std::endl;                                                         \
                    str << "File:       "                                                                                 \
                        << "[ " << __FILE__ << " ]" << std::endl;                                                         \
                    str << "-------------------------" << std::endl;                                                      \
                    auto result = MessageBox(NULL, str.str().data(), L"Throw", MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION); \
                    switch (result) {                                                                                     \
                    case IDRETRY:                                                                                         \
                    case IDABORT:                                                                                         \
                        throw std::runtime_error(message);                                                                \
                        break;                                                                                            \
                    }                                                                                                     \
                }                                                                                                         \
            }
#    elif defined(ADH_APPLE)
#        include <Cocoa/Cocoa.h>
#        define ADH_THROW(expression, message)                                \
            {                                                                 \
                if (!(expression)) {                                          \
                    std::stringstream str;                                    \
                    str << "--- Throw! ---"                                   \
                        << "\n"                                               \
                        << std::endl;                                         \
                    str << "Message:    "                                     \
                        << message                                            \
                        << "\n"                                               \
                        << std::endl;                                         \
                    str << "-------------------------" << std::endl;          \
                    std::stringstream str2;                                   \
                                                                              \
                    str2 << "Function: "                                      \
                         << "[ " << __func__ << " ]" << std::endl;            \
                    str2 << "Line: "                                          \
                         << "[ " << __LINE__ << " ]" << std::endl;            \
                    str2 << "File: "                                          \
                         << "[ " << __FILE__ << " ]" << std::endl;            \
                                                                              \
                    MessageBox(message, str.str().data(), str2.str().data()); \
                }                                                             \
            }
namespace adh {
    void MessageBox(const char* message, const char* str1, const char* str2);
}
#    else
#        define ADH_THROW(expression, message)                             \
            {                                                              \
                if (!(expression)) {                                       \
                    std::cout << "--- Throw! ---"                          \
                              << "\n"                                      \
                              << std::endl;                                \
                    std::cout << "Message:    " << message << "\n"         \
                              << std::endl;                                \
                    std::cout << "Function:   "                            \
                              << "[ " << __func__ << " ]" << std::endl;    \
                    std::cout << "Line:       "                            \
                              << "[ " << __LINE__ << " ]" << std::endl;    \
                    std::cout << "File:       "                            \
                              << "[ " << __FILE__ << " ]" << std::endl;    \
                    std::cout << "-------------------------" << std::endl; \
                    throw std::runtime_error(message);                     \
                }                                                          \
            }
#    endif // Platform
#else
#    define ADH_THROW(expression, message) (void)(expression)
#endif // ADH_DEBUG

#if defined(ADH_NOEXCEPT)
#    undef ADH_NOEXCEPT
#endif
#if defined(ADH_DEBUG)
#    define ADH_NOEXCEPT
#else
#    define ADH_NOEXCEPT noexcept
#endif // ADH_DEBUG

#if !defined(ADH_STACK_ALLOC)
#    undef ADH_STACK_ALLOC
#endif
#if defined(ADH_WINDOWS)
#    define ADH_STACK_ALLOC _malloca
#else
#    define ADH_STACK_ALLOC alloca
#endif

#if defined(ADH_OFFSET)
#    undef ADH_OFFSET
#endif
#define ADH_OFFSET(x, y) \
    std::size_t { reinterpret_cast<std::size_t>(&((static_cast<x*>(nullptr)->y))) }

#if defined(ADH_TO_STRING)
#    undef ADH_TO_STRING
#endif
#define ADH_TO_STRING(x) #x

#if defined(ADH_LOG)
#    undef ADH_LOG
#endif
#define ADH_LOG(x) std::cout << x << std::endl
