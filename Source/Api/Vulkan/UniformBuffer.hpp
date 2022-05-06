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
#include "Buffer.hpp"
#include <Std/Array.hpp>
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        class UniformBuffer {
          public:
            UniformBuffer() noexcept;

            UniformBuffer(const void* data, std::size_t size, std::uint32_t count, VkBufferUsageFlagBits bufferUsage);

            template <typename T>
            inline UniformBuffer(const Array<T>& data, std::uint32_t count, VkBufferUsageFlagBits bufferUsage);

            UniformBuffer(const UniformBuffer& rhs) = delete;

            UniformBuffer& operator=(const UniformBuffer& rhs) = delete;

            UniformBuffer(UniformBuffer&& rhs) noexcept;

            UniformBuffer& operator=(UniformBuffer&& rhs) noexcept;

            ~UniformBuffer();

            template <typename T>
            inline void Create(const Array<T>& data, std::uint32_t count, VkBufferUsageFlagBits bufferUsage);

            void Create(const void* data, std::size_t size, std::uint32_t count, VkBufferUsageFlagBits bufferUsage);

            void Destroy() noexcept;

            void Update(std::uint32_t imageIndex = 0u) noexcept;

            void Update(const void* data, std::uint32_t imageIndex = 0u) noexcept;

            VkDescriptorBufferInfo GetDescriptor() noexcept;

            const VkDescriptorBufferInfo GetDescriptor() const noexcept;

            std::uint64_t GetSize() const noexcept;

            void* GetMappedPtr() noexcept;

            const void* GetMappedPtr() const noexcept;

            operator VkBuffer() noexcept;

            operator const VkBuffer() const noexcept;

          private:
            void MoveConstruct(UniformBuffer&& rhs) noexcept;

            void Clear() noexcept;

          private:
            Buffer m_Buffer;
            VkDescriptorBufferInfo m_Descriptor;
            const void* m_Data;
            void* m_MappedPtr;
        };
    } // namespace vk
} // namespace adh

namespace adh {
    namespace vk {
        template <typename T>
        UniformBuffer::UniformBuffer(const Array<T>& data, std::uint32_t count, VkBufferUsageFlagBits bufferUsage) {
            Create(data.GetData(), sizeof(Array<T>::Type) * data.GetSize(), count, bufferUsage);
        }

        template <typename T>
        void UniformBuffer::Create(const Array<T>& data, std::uint32_t count, VkBufferUsageFlagBits bufferUsage) {
            Create(data.GetData(), sizeof(Array<T>::Type) * data.GetSize(), count, bufferUsage);
        }
    } // namespace vk
} // namespace adh
