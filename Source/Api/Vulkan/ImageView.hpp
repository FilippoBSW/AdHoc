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
#include <iostream>
#include <vulkan/vulkan.h>

#include <Utility.hpp>

namespace adh {
    namespace vk {
        class ImageView {
            ImageView() = default;

            ImageView(VkImage image,
                      VkImageViewType viewType,
                      VkFormat format,
                      VkImageAspectFlagBits imageAspectFlags,
                      std::uint32_t levelCount,
                      std::uint32_t layerCount,
                      std::uint32_t baseArrayLayer) ADH_NOEXCEPT;

            ImageView(const ImageView& rhs) = delete;

            ImageView& operator=(const ImageView& rhs) = delete;

            ImageView(ImageView&& rhs) noexcept;

            ImageView& operator=(ImageView&& rhs) noexcept;

            ~ImageView();

            void Create(VkImage image,
                        VkImageViewType viewType,
                        VkFormat format,
                        VkImageAspectFlagBits imageAspectFlags,
                        std::uint32_t levelCount,
                        std::uint32_t layerCount,
                        std::uint32_t baseArrayLayer) ADH_NOEXCEPT;

            void Destroy();

            VkImageView& Get() noexcept;

            const VkImageView& Get() const noexcept;

            operator VkImageView() noexcept;

            operator const VkImageView() const noexcept;

          private:
            void MoveConstruct(ImageView&& rhs) noexcept;

            void Clear() noexcept;

          private:
            VkImageView mImageView{ VK_NULL_HANDLE };
        };
    } // namespace vk
} // namespace adh
