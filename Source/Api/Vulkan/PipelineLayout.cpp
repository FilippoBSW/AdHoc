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

#include "PipelineLayout.hpp"
#include "Context.hpp"
#include "Initializers.hpp"

namespace adh {
    namespace vk {
        PipelineLayout::PipelineLayout() noexcept : m_PipelineLayout{ VK_NULL_HANDLE } {
        }

        PipelineLayout::PipelineLayout(PipelineLayout&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        PipelineLayout& PipelineLayout::operator=(PipelineLayout&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));

            return *this;
        }

        PipelineLayout::~PipelineLayout() {
            Clear();
        }

        void PipelineLayout::Create() ADH_NOEXCEPT {
            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
                initializers::PipelineLayoutCreateInfo(
                    static_cast<std::uint32_t>(m_DescriptorSetLayout.GetSize()),
                    m_DescriptorSetLayout.GetData(),
                    static_cast<std::uint32_t>(m_PushConstants.GetSize()),
                    m_PushConstants.GetData())
            };

            ADH_THROW(vkCreatePipelineLayout(Context::Get()->GetDevice(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout) == VK_SUCCESS,
                      "Failed to create pipeline layout!");
        }

        void PipelineLayout::Destroy() noexcept {
            Clear();
        }

        void PipelineLayout::AddBinding(std::uint32_t binding,
                                        VkDescriptorType type,
                                        std::uint32_t count,
                                        VkShaderStageFlags shaderStage,
                                        const VkSampler* immutableSampler) {
            m_LayoutBindings.EmplaceBack(initializers::DescriptorSetLayoutBinding(binding, type, count, shaderStage, immutableSampler));
        }

        void PipelineLayout::AddPushConstant(VkShaderStageFlags shaderStage, std::uint32_t size, std::uint32_t offset) {
            m_PushConstants.EmplaceBack(initializers::PushConstantRange(shaderStage, offset, size));
        }

        void PipelineLayout::CreateSet() ADH_NOEXCEPT {
            auto layoutCreateInfo{ initializers::DescriptorSetLayoutCreateInfo(m_LayoutBindings.GetData(),
                                                                               static_cast<std::uint32_t>(m_LayoutBindings.GetSize())) };
            auto& temp{ m_DescriptorSetLayout.EmplaceBack() };
            ADH_THROW(vkCreateDescriptorSetLayout(Context::Get()->GetDevice(), &layoutCreateInfo, nullptr, &temp) == VK_SUCCESS,
                      "Failed to create descriptor set layout!");
            m_LayoutBindings.Clear();
        }

        VkPipelineLayout PipelineLayout::GetPipelineLayout() noexcept {
            return m_PipelineLayout;
        }

        const VkPipelineLayout PipelineLayout::GetPipelineLayout() const noexcept {
            return m_PipelineLayout;
        }

        Array<VkDescriptorSetLayout>& PipelineLayout::GetSetLayout() noexcept {
            return m_DescriptorSetLayout;
        }

        const Array<VkDescriptorSetLayout>& PipelineLayout::GetSetLayout() const noexcept {
            return m_DescriptorSetLayout;
        }

        VkPushConstantRange* PipelineLayout::GetPushConstant() noexcept {
            return m_PushConstants.GetData();
        }

        const VkPushConstantRange* PipelineLayout::GetPushConstant() const noexcept {
            return m_PushConstants.GetData();
        }

        PipelineLayout::operator VkPipelineLayout() {
            return m_PipelineLayout;
        }

        PipelineLayout::operator const VkPipelineLayout() const {
            return m_PipelineLayout;
        }

        void PipelineLayout::MoveConstruct(PipelineLayout&& rhs) noexcept {
            m_LayoutBindings      = Move(rhs.m_LayoutBindings);
            m_DescriptorSetLayout = Move(rhs.m_DescriptorSetLayout);
            m_PipelineLayout      = rhs.m_PipelineLayout;

            rhs.m_PipelineLayout = VK_NULL_HANDLE;
        }

        void PipelineLayout::Clear() noexcept {
            auto device{ Context::Get()->GetDevice() };
            for (std::size_t i{}; i != m_DescriptorSetLayout.GetSize(); ++i) {
                vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout[i], nullptr);
                m_DescriptorSetLayout[i] = VK_NULL_HANDLE;
            }
            if (m_PipelineLayout != VK_NULL_HANDLE) {
                vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
            }
            m_LayoutBindings.Clear();
            m_DescriptorSetLayout.Clear();
            m_PushConstants.Clear();
            m_PipelineLayout = VK_NULL_HANDLE;
        }
    } // namespace vk
} // namespace adh
