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
#include "Image.hpp"
#include "Sampler.hpp"
#include "UniformBuffer.hpp"

#include <Std/Array.hpp>

#include <vulkan/vulkan.h>

#include <string>

namespace adh {
    namespace vk {
        class Texture2D {
          public:
            Texture2D() noexcept;

            Texture2D(
                const char* filePath,
                VkImageUsageFlagBits imageUsage,
                const Sampler* sampler,
                VkBool32 generateMinMap   = VK_FALSE,
                VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE);

            Texture2D(
                const void* data,
                std::size_t size,
                VkExtent2D extent,
                VkImageUsageFlagBits imageUsage,
                const Sampler* sampler,
                VkBool32 generateMinMap   = VK_FALSE,
                VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE);

            Texture2D(
                VkExtent2D extent,
                VkImageUsageFlagBits imageUsage,
                VkImageLayout imageLayout,
                const Sampler* sampler,
                VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE);

            Texture2D(
                const char* filePath,
                VkImageUsageFlagBits imageUsage,
                VkFilter filter,
                VkBool32 generateMinMap   = VK_FALSE,
                VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE);

            Texture2D(const Texture2D& rhs) = delete;

            Texture2D& operator=(const Texture2D& rhs) = delete;

            Texture2D(Texture2D&& rhs) noexcept;

            Texture2D& operator=(Texture2D&& rhs) noexcept;

            ~Texture2D() = default;

            void Create(
                const char* filePath,
                VkImageUsageFlagBits imageUsage,
                const Sampler* sampler,
                VkBool32 generateMinMap   = VK_FALSE,
                VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE);

            void Create(
                const void* data,
                std::size_t size,
                VkExtent2D extent,
                VkImageUsageFlagBits imageUsage,
                const Sampler* sampler,
                VkBool32 generateMinMap   = VK_FALSE,
                VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE);

            void Create(
                VkExtent2D extent,
                VkImageUsageFlagBits imageUsage,
                VkImageLayout imageLayout,
                const Sampler* sampler,
                VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE);

            void Create(
                const char* filePath,
                VkImageUsageFlagBits imageUsage,
                VkFilter filter,
                VkBool32 generateMinMap   = VK_FALSE,
                VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE);

            VkImage GetImage() noexcept;

            const VkImage GetImage() const noexcept;

            VkImageView GetImageView() noexcept;

            const VkImageView GetImageView() const noexcept;

            std::size_t GetWidth() const noexcept;

            std::size_t GetHeight() const noexcept;

            VkDescriptorImageInfo GetDescriptor() noexcept;

            const VkDescriptorImageInfo GetDescriptor() const noexcept;

            void SetLayout(VkImageLayout imageLayout) noexcept;

            std::uint32_t GetMipLevels() const noexcept;

            std::string& GetFilePath() noexcept {
                return mFilePath;
            }

            void UpdateSampler(bool isLinearFilter) {
                if (mIsLinearFilter != isLinearFilter) {
                    mIsLinearFilter      = isLinearFilter;
                    m_Descriptor.sampler = m_DefaultSamplers[!mIsLinearFilter];
                }
            }

            void Destroy() noexcept;

            operator VkImageView() noexcept;

            operator const VkImageView() const noexcept;

            static void InitializeDefaultSamplers();

            static void CleanUpDefaultSamplers();

          private:
            VkImageUsageFlagBits SelectImageUsage(VkImageUsageFlagBits imageUsage, VkBool32 generateMinMap) noexcept;

            void SelectImageLayout(VkImageUsageFlagBits imageUsage) noexcept;

            void CreateImage(
                const UniformBuffer& staging,
                VkBool32 generateMinMap,
                VkImageUsageFlagBits usageFlag,
                VkSharingMode sharingMode);

            void CreateImage(
                VkImageUsageFlagBits usageFlag,
                VkImageLayout imageLayout,
                VkSharingMode sharingMode);

            void GenerateMipMaps() ADH_NOEXCEPT;

            void GenerateImageBarriers(VkCommandBuffer commandBuffer, std::int32_t mipWidth, std::int32_t mipHeight) noexcept;

            void BlitImage(VkCommandBuffer commandBuffer, std::int32_t index, std::int32_t mipWidth, std::int32_t mipHeight) noexcept;

            void InitializeDescriptor(const Sampler* sampler) noexcept;

            void MoveConstruct(Texture2D&& rhs) noexcept;

            void Clear() noexcept;

          public:
            std::string mFileName;

          private:
            Image m_Image;
            VkDescriptorImageInfo m_Descriptor;
            VkExtent2D m_Extent;
            std::uint32_t m_MipLevels;

            std::string mFilePath;
            bool mIsLinearFilter{ true };

            inline static Array<Sampler> m_DefaultSamplers;
        };
    } // namespace vk
} // namespace adh
