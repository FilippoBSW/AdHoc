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

#include <Utility.hpp>
#include <iostream>
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        class Framebuffer {
          public:
            Framebuffer() = default;

            Framebuffer(VkRenderPass renderPass,
                        std::uint32_t attachmentCount,
                        const VkImageView* attachments,
                        VkExtent2D extent,
                        std::uint32_t layers);

            Framebuffer(const Framebuffer& rhs) = delete;

            Framebuffer& operator=(const Framebuffer& rhs) = delete;

            Framebuffer(Framebuffer&& rhs) noexcept;

            Framebuffer& operator=(Framebuffer&& rhs) noexcept;

            ~Framebuffer();

            void Create(VkRenderPass renderPass,
                        std::uint32_t attachmentCount,
                        const VkImageView* attachments,
                        VkExtent2D extent,
                        std::uint32_t layers) ADH_NOEXCEPT;

            void Destroy();

            VkFramebuffer& Get() noexcept;

            const VkFramebuffer& Get() const noexcept;

            operator VkFramebuffer() noexcept;

            operator const VkFramebuffer() const noexcept;

          private:
            void MoveConstruct(Framebuffer&& rhs) noexcept;

            void Clear() noexcept;

          private:
            VkFramebuffer mFramebuffer;
        };
    } // namespace vk
} // namespace adh
