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
#include "Attachments.hpp"
#include "Subpass.hpp"
#include <Std/Array.hpp>
#include <Utility.hpp>
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        class RenderPass {
          public:
            RenderPass() noexcept;

            RenderPass(const Attachment& attachment, const Subpass& subpass, VkRect2D renderArea, Array<VkClearValue> clearValues) ADH_NOEXCEPT;

            RenderPass(const RenderPass& rhs) = delete;

            RenderPass& operator=(const RenderPass& rhs) = delete;

            RenderPass(RenderPass&& rhs) noexcept;

            RenderPass& operator=(RenderPass&& rhs) noexcept;

            ~RenderPass();

            void Create(const Attachment& attachment, const Subpass& subpass, VkRect2D renderArea, Array<VkClearValue> clearValues) ADH_NOEXCEPT;

            void Destroy() noexcept;

            void Begin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer) noexcept;

            void End(VkCommandBuffer commandBuffer) noexcept;

            void UpdateRenderArea(VkRect2D renderArea) noexcept;

            void UpdateClearValues(Array<VkClearValue> clearValues) noexcept;

            void SetClearColor(float r, float g, float b, float a) noexcept;

            void SetClearDepthStencil(float depth, float stencil) noexcept;

            operator VkRenderPass() noexcept;

            operator const VkRenderPass() const noexcept;

          private:
            void MoveConstruct(RenderPass&& rhs) noexcept;

            void Clear() noexcept;

          private:
            VkRenderPass m_RenderPass;
            VkRect2D m_RenderArea;
            Array<VkClearValue> m_ClearValues;
        };
    } // namespace vk
} // namespace adh
