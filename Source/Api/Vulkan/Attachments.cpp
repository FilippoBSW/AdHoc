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

#include "Attachments.hpp"
#include "Context.hpp"
#include "Initializers.hpp"

namespace adh {
    namespace vk {
        void Attachment::AddDescription(VkFormat format,
                                        VkSampleCountFlagBits samplerCount,
                                        VkAttachmentLoadOp loadOp,
                                        VkAttachmentStoreOp storeOp,
                                        VkAttachmentLoadOp stencilLoadOp,
                                        VkAttachmentStoreOp stencilStoreOp,
                                        VkImageLayout finalLayout,
                                        VkImageLayout attachment) {
            m_Descriptions.EmplaceBack(initializers::AttachmentDescription(
                format,
                samplerCount,
                loadOp,
                storeOp,
                stencilLoadOp,
                stencilStoreOp,
                finalLayout));

            switch (attachment) {
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                m_ColorReferences.EmplaceBack(initializers::AttachmentReference(
                    static_cast<std::uint32_t>(m_Descriptions.GetSize() - 1u),
                    static_cast<VkImageLayout>(attachment)));
                break;

            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                m_DepthReferences.EmplaceBack(initializers::AttachmentReference(
                    static_cast<std::uint32_t>(m_Descriptions.GetSize() - 1u),
                    static_cast<VkImageLayout>(attachment)));
                break;
            }
        }

        void Attachment::AddInputReference(VkImageLayout attachment, std::uint32_t index) {
            m_InputReferences.EmplaceBack(initializers::AttachmentReference(
                index,
                static_cast<VkImageLayout>(attachment)));
        }

        Array<VkAttachmentDescription>& Attachment::GetDescriptions() noexcept {
            return m_Descriptions;
        }

        const Array<VkAttachmentDescription>& Attachment::GetDescriptions() const noexcept {
            return m_Descriptions;
        }

        Array<VkAttachmentReference>& Attachment::GetColorReferences() noexcept {
            return m_ColorReferences;
        }

        const Array<VkAttachmentReference>& Attachment::GetColorReferences() const noexcept {
            return m_ColorReferences;
        }

        Array<VkAttachmentReference>& Attachment::GetDepthReferences() noexcept {
            return m_DepthReferences;
        }

        const Array<VkAttachmentReference>& Attachment::GetDepthReferences() const noexcept {
            return m_DepthReferences;
        }

        Array<VkAttachmentReference>& Attachment::GetInputReferences() noexcept {
            return m_InputReferences;
        }

        const Array<VkAttachmentReference>& Attachment::GetInputReferences() const noexcept {
            return m_InputReferences;
        }
    } // namespace vk
} // namespace adh
