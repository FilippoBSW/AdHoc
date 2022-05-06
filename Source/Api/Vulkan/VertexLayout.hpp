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
#include <Std/Array.hpp>
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        class VertexLayout {
          public:
            VertexLayout() noexcept;

            VertexLayout(const VertexLayout& rhs) = delete;

            VertexLayout& operator=(const VertexLayout& rhs) = delete;

            VertexLayout(VertexLayout&& rhs) noexcept;

            VertexLayout& operator=(VertexLayout&& rhs) noexcept;

            ~VertexLayout() = default;

            void AddBinding(std::uint32_t binding, std::uint32_t stride, VkVertexInputRate inputRate) noexcept;

            void AddAttribute(std::uint32_t location, std::uint32_t binding, VkFormat format, std::uint32_t offset) noexcept;

            void Create() noexcept;

            VkPipelineVertexInputStateCreateInfo& Get() noexcept;

            const VkPipelineVertexInputStateCreateInfo& Get() const noexcept;

          private:
            void MoveConstruct(VertexLayout&& rhs) noexcept;

          private:
            Array<VkVertexInputBindingDescription> m_BindingDescription;
            Array<VkVertexInputAttributeDescription> m_AtttributeDescription;
            VkPipelineVertexInputStateCreateInfo m_PipelineVertexInputStateCreateInfo;
        };
    } // namespace vk
} // namespace adh
