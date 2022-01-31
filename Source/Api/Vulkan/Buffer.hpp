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
#include "Allocator.hpp"
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        class Buffer {
          public:
            Buffer() noexcept = default;

            Buffer(std::uint64_t size, std::size_t count, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryProperty);

            Buffer(const Buffer&) = delete;

            Buffer& operator=(const Buffer&) = delete;

            Buffer(Buffer&& rhs) noexcept;

            Buffer& operator=(Buffer&& rhs) noexcept;

            ~Buffer();

            void Create(std::uint64_t size, std::size_t count, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryProperty);

            void Bind() ADH_NOEXCEPT;

            void Map(const void* data) ADH_NOEXCEPT;

            void Map(const void* data, void*& toMap) ADH_NOEXCEPT;

            void Unmap() noexcept;

            void Flush() ADH_NOEXCEPT;

            void Destroy() noexcept;

            VkBuffer Get() noexcept;

            const VkBuffer Get() const noexcept;

            operator VkBuffer() noexcept;

            operator const VkBuffer() const noexcept;

          private:
            void MoveConstruct(Buffer&& rhs) noexcept;

            void Clear() noexcept;

          private:
            BufferData m_Buffer;
        };
    } // namespace vk
} // namespace adh
