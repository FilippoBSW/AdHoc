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

#include "Subpass.hpp"
#include "Initializers.hpp"

namespace adh {
    namespace vk {
        void Subpass::AddDescription(VkPipelineBindPoint bindPoint, const Attachment& attachment) {
            m_Descriptions.EmplaceBack(
                initializers::SubpassDescription(
                    static_cast<VkPipelineBindPoint>(bindPoint), // Bind point
                    attachment.GetColorReferences().GetSize(),   // color count
                    attachment.GetColorReferences().GetData(),   // color ptr
                    attachment.GetDepthReferences().GetData(),   // depth ptr
                    attachment.GetInputReferences().GetSize(),   // input count
                    attachment.GetInputReferences().GetData())); // input ptr
        }

        void Subpass::AddDependencies(
            std::uint32_t srcSubpass,
            std::uint32_t dstSubpass,
            VkPipelineStageFlagBits srcStageMask,
            VkPipelineStageFlagBits dstStageMask,
            VkAccessFlagBits srcAccessMask,
            VkAccessFlagBits dstAccessMask,
            VkDependencyFlags dependencyFlags) {
            m_Dependencies.EmplaceBack(
                initializers::SubpassDependency(
                    srcSubpass,
                    dstSubpass,
                    static_cast<VkPipelineStageFlags>(srcStageMask),
                    static_cast<VkPipelineStageFlags>(dstStageMask),
                    static_cast<VkAccessFlags>(srcAccessMask),
                    static_cast<VkAccessFlags>(dstAccessMask),
                    dependencyFlags));
        }

        Array<VkSubpassDescription>& Subpass::GetDescriptions() noexcept {
            return m_Descriptions;
        }

        const Array<VkSubpassDescription>& Subpass::GetDescriptions() const noexcept {
            return m_Descriptions;
        }

        Array<VkSubpassDependency>& Subpass::GetDependencies() noexcept {
            return m_Dependencies;
        }

        const Array<VkSubpassDependency>& Subpass::GetDependencies() const noexcept {
            return m_Dependencies;
        }
    } // namespace vk
} // namespace adh
