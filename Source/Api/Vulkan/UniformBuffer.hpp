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
