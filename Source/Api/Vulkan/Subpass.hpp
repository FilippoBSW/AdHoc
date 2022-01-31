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
#include "Attachments.hpp"
#include <Std/Array.hpp>
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        class Subpass {
          public:
            void AddDescription(VkPipelineBindPoint bindPoint, const Attachment& attachment);

            void AddDependencies(
                std::uint32_t srcSubpass,
                std::uint32_t dstSubpass,
                VkPipelineStageFlagBits srcStageMask,
                VkPipelineStageFlagBits dstStageMask,
                VkAccessFlagBits srcAccessMask,
                VkAccessFlagBits dstAccessMask,
                VkDependencyFlags dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT);

            Array<VkSubpassDescription>& GetDescriptions() noexcept;

            const Array<VkSubpassDescription>& GetDescriptions() const noexcept;

            Array<VkSubpassDependency>& GetDependencies() noexcept;

            const Array<VkSubpassDependency>& GetDependencies() const noexcept;

          private:
            Array<VkSubpassDescription> m_Descriptions;
            Array<VkSubpassDependency> m_Dependencies;
        };
    } // namespace vk
} // namespace adh
