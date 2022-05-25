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

#include "Framebuffer.hpp"
#include <Vulkan/Context.hpp>
#include <Vulkan/Initializers.hpp>

namespace adh {
    namespace vk {
        Framebuffer::Framebuffer(VkRenderPass renderPass,
                                 std::uint32_t attachmentCount,
                                 const VkImageView* attachments,
                                 VkExtent2D extent,
                                 std::uint32_t layers) {
            Create(renderPass, attachmentCount, attachments, extent, layers);
        }

        Framebuffer::Framebuffer(Framebuffer&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        Framebuffer& Framebuffer::operator=(Framebuffer&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));
            return *this;
        }

        Framebuffer::~Framebuffer() {
            Clear();
        }

        void Framebuffer::Create(VkRenderPass renderPass,
                                 std::uint32_t attachmentCount,
                                 const VkImageView* attachments,
                                 VkExtent2D extent,
                                 std::uint32_t layers) ADH_NOEXCEPT {
            auto info{ initializers::FramebufferCreateInfo(renderPass, attachmentCount, attachments, extent, layers) };
            ADH_THROW(vkCreateFramebuffer(Context::Get()->GetDevice(), &info, nullptr, &mFramebuffer) == VK_SUCCESS,
                      "Failed to create frame buffers!");
        }

        void Framebuffer::Destroy() {
            Clear();
        }

        VkFramebuffer& Framebuffer::Get() noexcept {
            return mFramebuffer;
        }

        const VkFramebuffer& Framebuffer::Get() const noexcept {
            return mFramebuffer;
        }

        Framebuffer::operator VkFramebuffer() noexcept {
            return mFramebuffer;
        }

        Framebuffer::operator const VkFramebuffer() const noexcept {
            return mFramebuffer;
        }

        void Framebuffer::MoveConstruct(Framebuffer&& rhs) noexcept {
            mFramebuffer     = rhs.mFramebuffer;
            rhs.mFramebuffer = VK_NULL_HANDLE;
        }

        void Framebuffer::Clear() noexcept {
            if (mFramebuffer != VK_NULL_HANDLE) {
                vkDestroyFramebuffer(Context::Get()->GetDevice(), mFramebuffer, nullptr);
                mFramebuffer = VK_NULL_HANDLE;
            }
        }

    } // namespace vk

} // namespace adh
