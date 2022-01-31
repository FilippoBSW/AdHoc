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

#include "VertexBuffer.hpp"
#include "Context.hpp"
#include "Memory.hpp"
#include "Tools.hpp"

namespace adh {
    namespace vk {
        VertexBuffer::VertexBuffer(const void* data, std::size_t size) {
            Create(data, size);
        }

        VertexBuffer::VertexBuffer(VertexBuffer&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        VertexBuffer& VertexBuffer::operator=(VertexBuffer&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));
            return *this;
        }

        VertexBuffer::~VertexBuffer() {
            Clear();
        }

        void VertexBuffer::Create(const void* data, std::size_t size) {
            if (tools::IsUniformMemoryAccess(Context::Get()->GetPhysicalDevice())) {
                m_Buffer.Create(size, 1u, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
                void* ptr;
                m_Buffer.Map(data, ptr);
                m_Buffer.Unmap();
            } else {
                Buffer vertexStaging(size, 1u, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
                void* ptr;
                vertexStaging.Map(data, ptr);
                vertexStaging.Unmap();
                m_Buffer.Create(size, 1u, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
                CopyBuffer(vertexStaging, m_Buffer, size);
                vertexStaging.Destroy();
            }
        }

        void VertexBuffer::Bind(VkCommandBuffer commandBuffer) noexcept {
            VkDeviceSize offsets[]{ 0u };
            VkBuffer buffer{ m_Buffer };
            vkCmdBindVertexBuffers(commandBuffer, 0u, 1u, &buffer, offsets);
        }

        void VertexBuffer::Destroy() noexcept {
            Clear();
        }

        VertexBuffer::operator bool() const noexcept {
            return m_Buffer != VK_NULL_HANDLE;
        }

        void VertexBuffer::MoveConstruct(VertexBuffer&& rhs) noexcept {
            m_Buffer = Move(rhs.m_Buffer);
        }

        void VertexBuffer::Clear() noexcept {
            m_Buffer.Destroy();
        }
    } // namespace vk
} // namespace adh
