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
#include "DeviceQueues.hpp"
#include <Std/Array.hpp>
#include <Utility.hpp>

#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        class CommandBuffer {
          public:
            CommandBuffer() noexcept = default;

            CommandBuffer(VkCommandBufferLevel bufferLevel,
                          std::uint32_t count,
                          VkCommandPoolCreateFlagBits poolCreateFlag,
                          DeviceQueues::Family queueFamily) noexcept;

            CommandBuffer(const CommandBuffer& rhs) = delete;

            CommandBuffer& operator=(const CommandBuffer& rhs) = delete;

            CommandBuffer(CommandBuffer&& rhs) noexcept;

            CommandBuffer& operator=(CommandBuffer&& rhs) noexcept;

            ~CommandBuffer() = default;

            void Create(VkCommandBufferLevel bufferLevel,
                        std::uint32_t count,
                        VkCommandPoolCreateFlagBits poolCreateFlag,
                        DeviceQueues::Family queueFamily);

            VkCommandBuffer Begin(std::size_t index = 0u, VkCommandBufferUsageFlagBits usageFlag = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

            void End(std::size_t index = 0u);

            void Reset();

            void Reset(std::size_t index);

            void Free();

            void Free(std::size_t index);

            void Destroy() noexcept;

            VkCommandBuffer& operator[](std::size_t index) noexcept;

            const VkCommandBuffer& operator[](std::size_t index) const noexcept;

          private:
            void MoveConstruct(CommandBuffer&& rhs);

            void Clear() noexcept;

          private:
            Array<VkCommandBuffer> m_CommandBuffers;
            VkCommandPool m_CommandPool{ VK_NULL_HANDLE };
        };
    } // namespace vk
} // namespace adh
