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

#pragma once
#include <Std/Array.hpp>
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        class Attachment {
          public:
            void AddDescription(VkFormat format,
                                VkSampleCountFlagBits samplerCount,
                                VkAttachmentLoadOp loadOp,
                                VkAttachmentStoreOp storeOp,
                                VkAttachmentLoadOp stencilLoadOp,
                                VkAttachmentStoreOp stencilStoreOp,
                                VkImageLayout finalLayout,
                                VkImageLayout attachment);

            void AddInputReference(VkImageLayout attachment, std::uint32_t index);

            Array<VkAttachmentDescription>& GetDescriptions() noexcept;

            const Array<VkAttachmentDescription>& GetDescriptions() const noexcept;

            Array<VkAttachmentReference>& GetColorReferences() noexcept;

            const Array<VkAttachmentReference>& GetColorReferences() const noexcept;

            Array<VkAttachmentReference>& GetDepthReferences() noexcept;

            const Array<VkAttachmentReference>& GetDepthReferences() const noexcept;

            Array<VkAttachmentReference>& GetInputReferences() noexcept;

            const Array<VkAttachmentReference>& GetInputReferences() const noexcept;

          private:
            Array<VkAttachmentDescription> m_Descriptions;
            Array<VkAttachmentReference> m_ColorReferences;
            Array<VkAttachmentReference> m_DepthReferences;
            Array<VkAttachmentReference> m_InputReferences;
        };
    } // namespace vk
} // namespace adh
