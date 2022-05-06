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
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        class Scissor {
          public:
            Scissor() noexcept = default;

            Scissor(std::uint32_t width, std::uint32_t height, std::int32_t offsetX = 0, std::int32_t offsetY = 0) noexcept;

            Scissor(VkExtent2D extent, VkOffset2D offset = {}) noexcept;

            Scissor(const VkRect2D& rect) noexcept;

            void Create(std::uint32_t width, std::uint32_t height, std::int32_t offsetX = 0, std::int32_t offsetY = 0) noexcept;

            void Create(VkExtent2D extent, VkOffset2D offset = {}) noexcept;

            void Create(const VkRect2D& rect) noexcept;

            void Update(std::uint32_t width, std::uint32_t height, std::int32_t offsetX = 0, std::int32_t offsetY = 0) noexcept;

            void Update(VkExtent2D extent, VkOffset2D offset = {}) noexcept;

            void Update(const VkRect2D& rect) noexcept;

            void Set(VkCommandBuffer commandBuffer) noexcept;

          private:
            VkRect2D m_Scissor;
        };
    } // namespace vk
} // namespace adh
