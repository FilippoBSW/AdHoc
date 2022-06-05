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

#include "Surface.hpp"
#include "Initializers.hpp"

#include <Std/Utility.hpp>
#include <Utility.hpp>

namespace adh {
    namespace vk {
        Surface::Surface() noexcept : m_Surface{ VK_NULL_HANDLE },
                                      m_Instance{ VK_NULL_HANDLE } {
        }

        Surface::Surface(VkInstance instance, const Window& window) ADH_NOEXCEPT {
            Create(instance, window);
        }

        Surface::Surface(Surface&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        Surface& Surface::operator=(Surface&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));

            return *this;
        }

        Surface::~Surface() {
            Clear();
        }

        void Surface::Create(VkInstance instance, const Window& window) ADH_NOEXCEPT {
            m_Instance = instance;
#if defined(ADH_WINDOWS)
            auto info{ initializers::Win32SurfaceCreateInfo(window.GetHandle(), window.GetInstance()) };
            ADH_THROW(vkCreateWin32SurfaceKHR(instance, &info, nullptr, &m_Surface) == VK_SUCCESS,
                      "Failed to create window surface!");
#elif defined(ADH_APPLE)
            auto info{ initializers::MacOSSurfaceCreateInfo(window.GetHandle()) };
            ADH_THROW(vkCreateMacOSSurfaceMVK(instance, &info, nullptr, &m_Surface) == VK_SUCCESS,
                      "Failed to create window surface!");
#elif defined(ADH_LINUX)
            auto info{ initializers::XcbSurfaceCreateInfo(window.GetConnection(), window.GetWindow()) };
            ADH_THROW(vkCreateXcbSurfaceKHR(instance, &info, nullptr, &m_Surface) == VK_SUCCESS,
                      "Failed to create window surface!");
            // auto info{ initializers::XlibSurfaceCreateInfo(window.GetDisplay(), window.GetWindow()) };
            // ADH_THROW(vkCreateXlibSurfaceKHR(instance, &info, nullptr, &m_Surface) == VK_SUCCESS,
            //           "Failed to create window surface!");
#endif
        }

        void Surface::Destroy() noexcept {
            Clear();
        }

        Surface::operator VkSurfaceKHR() noexcept {
            return m_Surface;
        }

        Surface::operator const VkSurfaceKHR() const noexcept {
            return m_Surface;
        }

        void Surface::MoveConstruct(Surface&& rhs) noexcept {
            m_Surface     = rhs.m_Surface;
            rhs.m_Surface = VK_NULL_HANDLE;
        }

        void Surface::Clear() noexcept {
            if (m_Surface != VK_NULL_HANDLE) {
                vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
                m_Surface = VK_NULL_HANDLE;
            }
        }
    } // namespace vk
} // namespace adh
