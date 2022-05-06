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

#include "Buffer.hpp"
#include <Std/Utility.hpp>

namespace adh {
    namespace vk {
        Buffer::Buffer(std::uint64_t size, std::size_t count, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryProperty) {
            Create(size, count, usage, memoryProperty);
        }

        Buffer::Buffer(Buffer&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        Buffer& Buffer::operator=(Buffer&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));

            return *this;
        }

        Buffer::~Buffer() {
            Clear();
        }

        void Buffer::Create(std::uint64_t size, std::size_t count, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryProperty) {
            ADH_THROW(m_Buffer.buffer == VK_NULL_HANDLE, "Calling create on an already created buffer!");
            Allocator::CreateBuffer(static_cast<VkDeviceSize>(size * count),
                                    usage,
                                    memoryProperty,
                                    m_Buffer);
            Bind();
        }

        void Buffer::Bind() ADH_NOEXCEPT {
            Allocator::BindBuffer(m_Buffer);
        }

        void Buffer::Map(const void* data) ADH_NOEXCEPT {
            void* ptr;
            Allocator::Map(data, m_Buffer.memoryData.tail - m_Buffer.memoryData.head, m_Buffer.memoryData.head, m_Buffer.memoryData.memoryBlock->deviceMemory, ptr);
        }

        void Buffer::Map(const void* data, void*& toMap) ADH_NOEXCEPT {
            Allocator::Map(data, m_Buffer.memoryData.tail - m_Buffer.memoryData.head, m_Buffer.memoryData.head, m_Buffer.memoryData.memoryBlock->deviceMemory, toMap);
        }

        void Buffer::Unmap() noexcept {
            Allocator::Unmap(m_Buffer.memoryData.memoryBlock->deviceMemory);
        }

        void Buffer::Flush() ADH_NOEXCEPT {
            Allocator::Flush(m_Buffer.memoryData.tail - m_Buffer.memoryData.head, m_Buffer.memoryData.head, m_Buffer.memoryData.memoryBlock->deviceMemory);
        }

        void Buffer::Destroy() noexcept {
            Clear();
        }

        VkBuffer Buffer::Get() noexcept {
            return m_Buffer.buffer;
        }

        const VkBuffer Buffer::Get() const noexcept {
            return m_Buffer.buffer;
        }

        Buffer::operator VkBuffer() noexcept {
            return m_Buffer.buffer;
        }

        Buffer::operator const VkBuffer() const noexcept {
            return m_Buffer.buffer;
        }

        void Buffer::MoveConstruct(Buffer&& rhs) noexcept {
            m_Buffer     = rhs.m_Buffer;
            rhs.m_Buffer = {};
        }

        void Buffer::Clear() noexcept {
            if (m_Buffer.buffer != VK_NULL_HANDLE) {
                Allocator::DestroyBuffer(m_Buffer);
                m_Buffer = {};
            }
        }
    } // namespace vk
} // namespace adh
