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
#include <Utility.hpp>
#include <Window.hpp>

#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        class Surface {
          public:
            Surface() noexcept;

            Surface(VkInstance instance, const Window& window) ADH_NOEXCEPT;

            Surface(const Surface&) = delete;

            Surface& operator=(const Surface&) = delete;

            Surface(Surface&& rhs) noexcept;

            Surface& operator=(Surface&& rhs) noexcept;

            ~Surface();

            void Create(VkInstance instance, const Window& window) ADH_NOEXCEPT;

            void Destroy() noexcept;

            operator VkSurfaceKHR() noexcept;

            operator const VkSurfaceKHR() const noexcept;

          private:
            void MoveConstruct(Surface&& rhs) noexcept;

            void Clear() noexcept;

          private:
            VkSurfaceKHR m_Surface;
            VkInstance m_Instance;
        };
    } // namespace vk
} // namespace adh
