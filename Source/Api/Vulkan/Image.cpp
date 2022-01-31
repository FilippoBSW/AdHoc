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

#include "Image.hpp"

namespace adh {
    namespace vk {
        Image::Image(VkExtent3D extent,
                     VkFormat format,
                     VkImageTiling imageTiling,
                     VkImageType imageType,
                     VkImageCreateFlagBits imageCreateFlags,
                     std::uint32_t mipLevels,
                     std::uint32_t arrayLayers,
                     VkImageUsageFlagBits imageUsageFlag,
                     VkImageAspectFlagBits imageAspectFlag,
                     VkImageViewType imageViewType,
                     VkMemoryPropertyFlagBits memoryProperty,
                     VkSharingMode sharingMode) {
            Create(extent,
                   format,
                   imageTiling,
                   imageType,
                   imageCreateFlags,
                   mipLevels,
                   arrayLayers,
                   imageUsageFlag,
                   imageAspectFlag,
                   imageViewType,
                   memoryProperty,
                   sharingMode);
        }

        Image::Image(Image&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        Image& Image::operator=(Image&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));

            return *this;
        }

        Image::~Image() {
            Clear();
        }

        void Image::Create(VkExtent3D extent,
                           VkFormat format,
                           VkImageTiling imageTiling,
                           VkImageType imageType,
                           VkImageCreateFlagBits imageCreateFlags,
                           std::uint32_t mipLevels,
                           std::uint32_t arrayLayers,
                           VkImageUsageFlagBits imageUsageFlag,
                           VkImageAspectFlagBits imageAspectFlag,
                           VkImageViewType imageViewType,
                           VkMemoryPropertyFlagBits memoryProperty,
                           VkSharingMode sharingMode) {
            Allocator::CreateImage({ extent.width, extent.height, extent.depth },
                                   format,
                                   imageTiling,
                                   imageType,
                                   imageCreateFlags,
                                   mipLevels,
                                   arrayLayers,
                                   imageUsageFlag,
                                   memoryProperty,
                                   sharingMode,
                                   m_Image);

            Bind();

            Allocator::CreateImageView(
                imageViewType,
                format,
                imageAspectFlag,
                mipLevels,
                arrayLayers,
                m_Image);
        }

        void Image::Bind() ADH_NOEXCEPT {
            Allocator::BindImage(m_Image);
        }

        void Image::Destroy() noexcept {
            Clear();
        }

        VkImage Image::GetImage() noexcept {
            return m_Image.image;
        }

        const VkImage Image::GetImage() const noexcept {
            return m_Image.image;
        }

        VkImageView Image::GetImageView() noexcept {
            return m_Image.imageView;
        }

        const VkImageView Image::GetImageView() const noexcept {
            return m_Image.imageView;
        }

        Image::operator VkImage() noexcept {
            return m_Image.image;
        }

        Image::operator const VkImage() const noexcept {
            return m_Image.image;
        }

        void Image::MoveConstruct(Image&& rhs) noexcept {
            m_Image     = rhs.m_Image;
            rhs.m_Image = {};
        }

        void Image::Clear() noexcept {
            if (m_Image.image != VK_NULL_HANDLE &&
                m_Image.imageView != VK_NULL_HANDLE) {
                Allocator::DestroyImage(m_Image);
                m_Image = {};
            }
        }
    } // namespace vk
} // namespace adh
