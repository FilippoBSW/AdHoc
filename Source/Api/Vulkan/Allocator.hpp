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
#include <Std/Array.hpp>
#include <Std/Function.hpp>
#include <Std/List.hpp>
#include <Utility.hpp>
#include <vulkan/vulkan.h>

#include <unordered_map>

namespace adh {
    namespace vk {
        struct MemoryType {
            VkDeviceSize alignment{};

            std::uint32_t typeBits{};

            std::uint32_t typeIndex{};

            bool operator==(const MemoryType& rhs) const {
                return alignment == rhs.alignment &&
                       typeBits == rhs.typeBits &&
                       typeIndex == rhs.typeIndex;
            }
        };

        struct MemoryTypeHash {
            std::size_t operator()(const MemoryType& memoryType) const {
                std::size_t h1{ std::hash<VkDeviceSize>()(memoryType.alignment) };
                std::size_t h2{ std::hash<std::uint32_t>()(memoryType.typeBits) };
                std::size_t h3{ std::hash<std::uint32_t>()(memoryType.typeIndex) };
                return h1 ^ h2 ^ h3;
            }
        };

        struct NextFreeBlock {
            NextFreeBlock() = default;

            NextFreeBlock(VkDeviceSize head, VkDeviceSize tail)
                : head{ head },
                  tail{ tail } {
            }
            VkDeviceSize head{};
            VkDeviceSize tail{};

            bool operator<(const NextFreeBlock& rhs) const {
                return head < rhs.head;
            }
        };

        struct MemoryBlock {
            Array<NextFreeBlock> freeBlocks;
            VkDeviceMemory deviceMemory{ VK_NULL_HANDLE };
        };

        struct MemoryData {
            VkDeviceSize head{};
            VkDeviceSize tail{};
            MemoryBlock* memoryBlock{};
        };

        struct BufferData {
            MemoryData memoryData{};
            VkBuffer buffer{ VK_NULL_HANDLE };
        };

        struct ImageData {
            MemoryData memoryData{};
            VkImage image{ VK_NULL_HANDLE };
            VkImageView imageView{ VK_NULL_HANDLE };
        };

        class Allocator {
          public:
            static void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryPropertyFlag, BufferData& bufferData);

            static void CreateImage(
                VkExtent3D extent,
                VkFormat format,
                VkImageTiling imageTiling,
                VkImageType imageType,
                VkImageCreateFlagBits imageCreateFlags,
                std::uint32_t mipLevels,
                std::uint32_t arrayLayers,
                VkImageUsageFlagBits imageUsageFlag,
                VkMemoryPropertyFlagBits memoryPropertyFlag,
                VkSharingMode sharingMode,
                ImageData& imageData);

            static void CreateImageView(
                VkImageViewType imageViewType,
                VkFormat format,
                VkImageAspectFlagBits imageAspectFlag,
                std::uint32_t levelCount,
                std::uint32_t layerCount,
                ImageData& imageData);

            static void BindImage(ImageData& imageData) ADH_NOEXCEPT;

            static void BindBuffer(BufferData& bufferData) ADH_NOEXCEPT;

            static void Map(const void* data, VkDeviceSize size, VkDeviceSize offset, VkDeviceMemory deviceMemory, void*& toMap) ADH_NOEXCEPT;

            static void Unmap(VkDeviceMemory deviceMemory) noexcept;

            static void Flush(VkDeviceSize size, VkDeviceSize offset, VkDeviceMemory deviceMemory) ADH_NOEXCEPT;

            static void DestroyBuffer(BufferData& bufferData) noexcept;

            static void DestroyImage(ImageData& imageData) noexcept;

            static void Destroy() noexcept;

            static void Flush() noexcept;

          private:
            Allocator() = default;

            static Allocator& GetInstance() noexcept;

            void CreateBuffer2(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryPropertyFlag, BufferData& bufferData);

            void CreateImage2(
                VkExtent3D extent,
                VkFormat format,
                VkImageTiling imageTiling,
                VkImageType imageType,
                VkImageCreateFlagBits imageCreateFlags,
                std::uint32_t mipLevels,
                std::uint32_t arrayLayers,
                VkImageUsageFlagBits imageUsageFlag,
                VkMemoryPropertyFlagBits memoryPropertyFlag,
                VkSharingMode sharingMode,
                ImageData& imageData);

            void CreateImageView2(
                VkImageViewType imageViewType,
                VkFormat format,
                VkImageAspectFlagBits imageAspectFlag,
                std::uint32_t levelCount,
                std::uint32_t layerCount,
                ImageData& imageData) ADH_NOEXCEPT;

            void MapMemory(const void* data, VkDeviceSize size, VkDeviceSize offset, VkDeviceMemory deviceMemory, void*& toMap) ADH_NOEXCEPT;

            void UnmapMemory(VkDeviceMemory deviceMemory) noexcept;

            void FlushMemory(VkDeviceSize size, VkDeviceSize offset, VkDeviceMemory deviceMemory) ADH_NOEXCEPT;

            void BindBufferMemory(BufferData& bufferData) ADH_NOEXCEPT;

            void BindImageMemory(ImageData& imageData) ADH_NOEXCEPT;

            VkDeviceSize GetAlignedMemory(VkDeviceSize size, VkDeviceSize alignment) noexcept;

            void DestroyBuffer2(BufferData& bufferData) noexcept;

            void DestroyImage2(ImageData& imageData) noexcept;

            void Emplace(VkMemoryRequirements memoryRequirements, std::uint32_t memoryTypeIndex, MemoryData& memoryData);

            void AllocateMemory(VkDeviceSize size, std::uint32_t memoryTypeIndex, MemoryBlock& block) ADH_NOEXCEPT;

            void UpdateMemoryData(VkDeviceSize size, MemoryData& memoryData, MemoryBlock& block) noexcept;

            void UpdateMemoryBlock(VkDeviceSize newBlockSize, VkMemoryRequirements& memoryRequirements, std::uint32_t memoryTypeIndex, MemoryBlock& block) noexcept;

            void Flush2() noexcept;

            void Clear() noexcept;

          private:
            std::unordered_map<MemoryType, List<MemoryBlock>, MemoryTypeHash> m_Blocks;
            VkBool32 m_IsInitialized{};
            VkDeviceSize m_BlockSize{ 512'000'000u };
            Array<Function<void()>> m_DestroyQueue;
        };
    } // namespace vk
} // namespace adh
