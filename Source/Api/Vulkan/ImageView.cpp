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

#include <Vulkan/Context.hpp>
#include <Vulkan/Initializers.hpp>

#include "ImageView.hpp"

namespace adh {
    namespace vk {
        ImageView::ImageView(VkImage image,
                             VkImageViewType viewType,
                             VkFormat format,
                             VkImageAspectFlagBits imageAspectFlags,
                             std::uint32_t levelCount,
                             std::uint32_t layerCount,
                             std::uint32_t baseArrayLayer) ADH_NOEXCEPT {
            Create(image, viewType, format, imageAspectFlags, levelCount, layerCount);
        }

        ImageView::ImageView(ImageView&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        ImageView& ImageView::operator=(ImageView&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));
            return *this;
        }

        ImageView::~ImageView() {
            Clear();
        }

        void ImageView::Create(VkImage image,
                               VkImageViewType viewType,
                               VkFormat format,
                               VkImageAspectFlagBits imageAspectFlags,
                               std::uint32_t levelCount,
                               std::uint32_t layerCount,
                               std::uint32_t baseArrayLayer) ADH_NOEXCEPT {
            auto info{ initializers::ImageViewCreateInfo(image, viewType, format, imageAspectFlags, levelCount, layerCount) };
            info.subresourceRange.baseArrayLayer = baseArrayLayer;

            ADH_THROW(vkCreateImageView(Context::Get()->GetDevice(), &info, nullptr, &mImageView) == VK_SUCCESS,
                      "Failed to create image view!");
        }

        void ImageView::Destroy() {
            Clear();
        }

        VkImageView& ImageView::Get() noexcept {
            return mImageView;
        }

        const VkImageView& ImageView::Get() const noexcept {
            return mImageView;
        }

        ImageView::operator VkImageView() noexcept {
            return mImageView;
        }

        ImageView::operator const VkImageView() const noexcept {
            return mImageView;
        }

        void ImageView::MoveConstruct(ImageView&& rhs) noexcept {
            mImageView     = rhs.mImageView;
            rhs.mImageView = VK_NULL_HANDLE;
        }

        void ImageView::Clear() noexcept {
            if (mImageView != VK_NULL_HANDLE) {
                vkDestroyImageView(Context::Get()->GetDevice(), mImageView, nullptr);
                mImageView = VK_NULL_HANDLE;
            }
        }
    } // namespace vk
} // namespace adh
