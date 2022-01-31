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

#include "VertexLayout.hpp"
#include "Initializers.hpp"

namespace adh {
    namespace vk {
        VertexLayout::VertexLayout() noexcept : m_PipelineVertexInputStateCreateInfo{} {
        }

        VertexLayout::VertexLayout(VertexLayout&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        VertexLayout& VertexLayout::operator=(VertexLayout&& rhs) noexcept {
            MoveConstruct(Move(rhs));
            return *this;
        }

        void VertexLayout::AddBinding(std::uint32_t binding, std::uint32_t stride, VkVertexInputRate inputRate) noexcept {
            m_BindingDescription.EmplaceBack(initializers::VertexInputBindingDescription(binding, stride, inputRate));
        }

        void VertexLayout::AddAttribute(std::uint32_t location, std::uint32_t binding, VkFormat format, std::uint32_t offset) noexcept {
            m_AtttributeDescription.EmplaceBack(initializers::VertexInputAttributeDescription(location, binding, format, offset));
        }

        void VertexLayout::Create() noexcept {
            m_PipelineVertexInputStateCreateInfo = initializers::PipelineVertexInputStateCreateInfo(m_BindingDescription, m_AtttributeDescription);
        }

        VkPipelineVertexInputStateCreateInfo& VertexLayout::Get() noexcept {
            return m_PipelineVertexInputStateCreateInfo;
        }

        const VkPipelineVertexInputStateCreateInfo& VertexLayout::Get() const noexcept {
            return m_PipelineVertexInputStateCreateInfo;
        }

        void VertexLayout::MoveConstruct(VertexLayout&& rhs) noexcept {
            m_BindingDescription                 = Move(rhs.m_BindingDescription);
            m_AtttributeDescription              = Move(rhs.m_AtttributeDescription);
            m_PipelineVertexInputStateCreateInfo = rhs.m_PipelineVertexInputStateCreateInfo;
        }
    } // namespace vk
} // namespace adh
