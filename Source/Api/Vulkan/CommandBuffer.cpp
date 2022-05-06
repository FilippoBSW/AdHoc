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

#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Context.hpp"
#include "Initializers.hpp"
#include <Utility.hpp>

namespace adh {
    namespace vk {
        CommandBuffer::CommandBuffer(VkCommandBufferLevel bufferLevel,
                                     std::uint32_t count,
                                     VkCommandPoolCreateFlagBits poolCreateFlag,
                                     DeviceQueues::Family queueFamily) noexcept {
            Create(bufferLevel, count, poolCreateFlag, queueFamily);
        }

        CommandBuffer::CommandBuffer(CommandBuffer&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        CommandBuffer& CommandBuffer::operator=(CommandBuffer&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));
            return *this;
        }

        void CommandBuffer::Create(VkCommandBufferLevel bufferLevel,
                                   std::uint32_t count,
                                   VkCommandPoolCreateFlagBits poolCreateFlag,
                                   DeviceQueues::Family queueFamily) {
            m_CommandPool = CommandPool::Get()->Get(poolCreateFlag, Context::Get()->GetQueue(queueFamily).index.value());
            auto info{ initializers::CommandBufferAllocateInfo(m_CommandPool, count, bufferLevel) };
            m_CommandBuffers.Resize(count);
            ADH_THROW(vkAllocateCommandBuffers(Context::Get()->GetDevice(), &info, m_CommandBuffers.GetData()) == VK_SUCCESS,
                      "Failed to allocate command buffers!");
        }

        VkCommandBuffer CommandBuffer::Begin(std::size_t index, VkCommandBufferUsageFlagBits usageFlag) {
            auto info{ initializers::CommandBufferBeginInfo(usageFlag) };
            ADH_THROW(vkBeginCommandBuffer(m_CommandBuffers[index], &info) == VK_SUCCESS,
                      "Failed to begin command buffer!");
            return m_CommandBuffers[index];
        }

        void CommandBuffer::End(std::size_t index) {
            ADH_THROW(vkEndCommandBuffer(m_CommandBuffers[index]) == VK_SUCCESS,
                      "Failed to end command buffer!");
        }

        void CommandBuffer::Reset() {
            for (std::size_t i{}; i != m_CommandBuffers.GetSize(); ++i) {
                ADH_THROW(vkResetCommandBuffer(m_CommandBuffers[i], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT),
                          "Failed to reset command buffer!");
            }
        }

        void CommandBuffer::Reset(std::size_t index) {
            ADH_THROW(vkResetCommandBuffer(m_CommandBuffers[index], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT),
                      "Failed to reset command buffer!");
        }

        void CommandBuffer::Free() {
            vkFreeCommandBuffers(Context::Get()->GetDevice(), m_CommandPool, static_cast<std::uint32_t>(m_CommandBuffers.GetSize()), m_CommandBuffers.GetData());
        }

        void CommandBuffer::Free(std::size_t index) {
            vkFreeCommandBuffers(Context::Get()->GetDevice(), m_CommandPool, 1, &m_CommandBuffers[index]);
        }

        void CommandBuffer::Destroy() noexcept {
            Clear();
        }

        VkCommandBuffer& CommandBuffer::operator[](std::size_t index) noexcept {
            return m_CommandBuffers[index];
        }

        const VkCommandBuffer& CommandBuffer::operator[](std::size_t index) const noexcept {
            return m_CommandBuffers[index];
        }

        void CommandBuffer::MoveConstruct(CommandBuffer&& rhs) {
            m_CommandBuffers = Move(rhs.m_CommandBuffers);
            m_CommandPool    = rhs.m_CommandPool;
        }

        void CommandBuffer::Clear() noexcept {
            if (!m_CommandBuffers.IsEmpty()) {
                m_CommandBuffers.Clear();
                m_CommandPool = VK_NULL_HANDLE;
            }
        }
    } // namespace vk
} // namespace adh
