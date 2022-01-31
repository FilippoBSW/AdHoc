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

///*************************************************************************
///
/// Copyright (C) 2021 Filippo Corinto Brunn Schulte Wissing
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
/// http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
///*************************************************************************

#include "RenderPass.hpp"
#include "Context.hpp"
#include "Initializers.hpp"

namespace adh {
    namespace vk {
        RenderPass::RenderPass() noexcept : m_RenderPass{ VK_NULL_HANDLE },
                                            m_RenderArea{} {
        }

        RenderPass::RenderPass(const Attachment& attachment, const Subpass& subpass, VkRect2D renderArea, Array<VkClearValue> clearValues) ADH_NOEXCEPT {
            Create(attachment, subpass, renderArea, Move(clearValues));
        }

        RenderPass::RenderPass(RenderPass&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        RenderPass& RenderPass::operator=(RenderPass&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));
            return *this;
        }

        RenderPass::~RenderPass() {
            Clear();
        }

        void RenderPass::Create(const Attachment& attachment, const Subpass& subpass, VkRect2D renderArea, Array<VkClearValue> clearValues) ADH_NOEXCEPT {
            auto info{
                initializers::RenderPassCreateInfo(
                    attachment.GetDescriptions(),
                    subpass.GetDescriptions(),
                    subpass.GetDependencies())
            };

            ADH_THROW(vkCreateRenderPass(Context::Get()->GetDevice(), &info, nullptr, &m_RenderPass) == VK_SUCCESS,
                      "Failed to create render pass!");

            m_RenderArea  = renderArea;
            m_ClearValues = Move(clearValues);
        }

        void RenderPass::Destroy() noexcept {
            Clear();
        }

        void RenderPass::Begin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer) noexcept {
            auto info{ initializers::RenderPassBeginInfo(m_RenderPass, framebuffer, m_RenderArea, m_ClearValues.GetSize(), m_ClearValues.GetData()) };

            vkCmdBeginRenderPass(
                commandBuffer,
                &info,
                VK_SUBPASS_CONTENTS_INLINE);
        }

        void RenderPass::End(VkCommandBuffer commandBuffer) noexcept {
            vkCmdEndRenderPass(commandBuffer);
        }

        void RenderPass::UpdateRenderArea(VkRect2D renderArea) noexcept {
            m_RenderArea = renderArea;
        }

        void RenderPass::UpdateClearValues(Array<VkClearValue> clearValues) noexcept {
            m_ClearValues = Move(clearValues);
        }

        void RenderPass::SetClearColor(float r, float g, float b, float a) noexcept {
            m_ClearValues[0] = { r, g, b, a };
        }

        void RenderPass::SetClearDepthStencil(float depth, float stencil) noexcept {
            m_ClearValues[1] = { depth, stencil };
        }

        RenderPass::operator VkRenderPass() noexcept {
            return m_RenderPass;
        }

        RenderPass::operator const VkRenderPass() const noexcept {
            return m_RenderPass;
        }

        void RenderPass::MoveConstruct(RenderPass&& rhs) noexcept {
            m_RenderPass     = rhs.m_RenderPass;
            rhs.m_RenderPass = VK_NULL_HANDLE;
        }

        void RenderPass::Clear() noexcept {
            if (m_RenderPass != VK_NULL_HANDLE) {
                vkDestroyRenderPass(Context::Get()->GetDevice(), m_RenderPass, nullptr);
                m_RenderPass = VK_NULL_HANDLE;
            }
        }
    } // namespace vk
} // namespace adh
