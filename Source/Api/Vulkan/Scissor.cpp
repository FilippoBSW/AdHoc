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

#include "Scissor.hpp"
#include "Initializers.hpp"

namespace adh {
    namespace vk {
        Scissor::Scissor(std::uint32_t width, std::uint32_t height, std::int32_t offsetX, std::int32_t offsetY) noexcept {
            Create(width, height, offsetX, offsetY);
        }

        Scissor::Scissor(VkExtent2D extent, VkOffset2D offset) noexcept {
            Create(extent, offset);
        }

        Scissor::Scissor(const VkRect2D& rect) noexcept {
            m_Scissor = rect;
        }

        void Scissor::Create(std::uint32_t width, std::uint32_t height, std::int32_t offsetX, std::int32_t offsetY) noexcept {
            m_Scissor = initializers::Rect2D({ width, height }, { offsetX, offsetY });
        }

        void Scissor::Create(VkExtent2D extent, VkOffset2D offset) noexcept {
            m_Scissor = initializers::Rect2D(extent, offset);
        }

        void Scissor::Create(const VkRect2D& rect) noexcept {
            m_Scissor = rect;
        }

        void Scissor::Update(std::uint32_t width, std::uint32_t height, std::int32_t offsetX, std::int32_t offsetY) noexcept {
            Create(width, height, offsetX, offsetY);
        }

        void Scissor::Update(VkExtent2D extent, VkOffset2D offset) noexcept {
            Create(extent, offset);
        }

        void Scissor::Update(const VkRect2D& rect) noexcept {
            Create(rect);
        }

        void Scissor::Set(VkCommandBuffer commandBuffer) noexcept {
            vkCmdSetScissor(commandBuffer, 0, 1, &m_Scissor);
        }
    } // namespace vk
} // namespace adh
