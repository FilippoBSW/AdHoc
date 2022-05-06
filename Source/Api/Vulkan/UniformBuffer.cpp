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

#include "UniformBuffer.hpp"
#include "Context.hpp"
#include "Tools.hpp"

namespace adh {
    namespace vk {
        UniformBuffer::UniformBuffer() noexcept : m_Descriptor{},
                                                  m_Data{},
                                                  m_MappedPtr{} {
        }

        UniformBuffer::UniformBuffer(const void* data, std::size_t size, std::uint32_t count, VkBufferUsageFlagBits bufferUsage) {
            Create(data, size, count, bufferUsage);
        }

        UniformBuffer::UniformBuffer(UniformBuffer&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        UniformBuffer& UniformBuffer::operator=(UniformBuffer&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));

            return *this;
        }

        UniformBuffer::~UniformBuffer() {
            Clear();
        }

        void UniformBuffer::Create(const void* data, std::size_t size, std::uint32_t count, VkBufferUsageFlagBits bufferUsage) {
            auto memoryProperty{ tools::IsUniformMemoryAccess(Context::Get()->GetPhysicalDevice()) ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
            m_Buffer.Create(size, count, bufferUsage, VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | memoryProperty));
            m_Data = data;
            m_Buffer.Map(m_Data, m_MappedPtr);
            m_Buffer.Unmap();
            m_Descriptor.buffer = m_Buffer;
            m_Descriptor.offset = 0u;
            m_Descriptor.range  = static_cast<VkDeviceSize>(size);
        }

        void UniformBuffer::Destroy() noexcept {
            Clear();
        }

        void UniformBuffer::Update(std::uint32_t imageIndex) noexcept {
            Update(m_Data, imageIndex);
        }

        void UniformBuffer::Update(const void* data, std::uint32_t imageIndex) noexcept {
            const std::size_t offset{ m_Descriptor.range * imageIndex };
            void* const ptr{ static_cast<char* const>(m_MappedPtr) + offset };
            std::memcpy(ptr, data, static_cast<std::size_t>(m_Descriptor.range));
        }

        VkDescriptorBufferInfo UniformBuffer::GetDescriptor() noexcept {
            return m_Descriptor;
        }

        const VkDescriptorBufferInfo UniformBuffer::GetDescriptor() const noexcept {
            return m_Descriptor;
        }

        std::uint64_t UniformBuffer::GetSize() const noexcept {
            return static_cast<std::uint64_t>(m_Descriptor.range);
        }

        void* UniformBuffer::GetMappedPtr() noexcept {
            return m_MappedPtr;
        }

        const void* UniformBuffer::GetMappedPtr() const noexcept {
            return m_MappedPtr;
        }

        UniformBuffer::operator VkBuffer() noexcept {
            return m_Buffer;
        }

        UniformBuffer::operator const VkBuffer() const noexcept {
            return m_Buffer;
        }

        void UniformBuffer::MoveConstruct(UniformBuffer&& rhs) noexcept {
            m_Buffer     = Move(rhs.m_Buffer);
            m_Descriptor = rhs.m_Descriptor;
            m_Data       = rhs.m_Data;
            m_MappedPtr  = rhs.m_MappedPtr;

            rhs.m_Descriptor = {};
            rhs.m_Data       = nullptr;
            rhs.m_MappedPtr  = nullptr;
        }

        void UniformBuffer::Clear() noexcept {
            m_Buffer.Destroy();
            m_Descriptor = {};
            m_Data       = nullptr;
            m_MappedPtr  = nullptr;
        }
    } // namespace vk
} // namespace adh
