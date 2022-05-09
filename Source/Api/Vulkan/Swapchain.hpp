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
#include <Std/Array.hpp>

#include <iostream>
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        class Swapchain {
          public:
          public:
            Swapchain() noexcept;

            Swapchain(std::uint32_t imageBuffersCount, VkFormat format, VkPresentModeKHR presentMode);

            Swapchain(const Swapchain&) = delete;

            Swapchain& operator=(const Swapchain&) = delete;

            Swapchain(Swapchain&& rhs) noexcept;

            Swapchain& operator=(Swapchain&& rhs) noexcept;

            ~Swapchain();

            void Create(std::uint32_t imageBuffersCount, VkFormat format, VkPresentModeKHR presentMode);

            void Destroy() noexcept;

            VkSwapchainKHR Get() noexcept;

            const VkSwapchainKHR Get() const noexcept;

            Array<VkImageView>& GetImageView() noexcept;

            const Array<VkImageView>& GetImageView() const noexcept;

            VkFormat GetFormat() const noexcept;

            VkExtent2D GetExtent() const noexcept;

            Image& GetColorBuffer() noexcept;

            Image& GetDepthBuffer() noexcept;

            std::uint32_t GetImageViewCount() const noexcept;

            operator VkSwapchainKHR() noexcept;

            operator const VkSwapchainKHR() const noexcept;

          private:
            void FindFormat(VkFormat format, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

            void FindPresentMode(VkPresentModeKHR presentMode, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

            void CreateSwapchain(VkSwapchainKHR oldSwapchain);

            void CreateImageViews();

            void CreateDepthBuffer();

            void MoveConstruct(Swapchain&& rhs) noexcept;

            void Clear();

          public:
            bool isValid{ false };

          private:
            VkSwapchainKHR m_Swapchain;
            VkSurfaceFormatKHR m_Format;
            VkPresentModeKHR m_PresentMode;
            VkExtent2D m_Extent;
            Array<VkImage> m_Images;
            Array<VkImageView> m_ImageViews;
            Image m_DepthBuffer;
            Image m_ColorBuffer;
            std::uint32_t m_ImageBuffersCount;
        };
    } // namespace vk
} // namespace adh
