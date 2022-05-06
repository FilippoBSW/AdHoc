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
#include <Utility.hpp>
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        class PipelineLayout {
          public:
            PipelineLayout() noexcept;

            PipelineLayout(const PipelineLayout&) = delete;

            PipelineLayout& operator=(const PipelineLayout&) = delete;

            PipelineLayout(PipelineLayout&& rhs) noexcept;

            PipelineLayout& operator=(PipelineLayout&& rhs) noexcept;

            ~PipelineLayout();

            void Create() ADH_NOEXCEPT;

            void Destroy() noexcept;

            void AddBinding(std::uint32_t binding, VkDescriptorType type, std::uint32_t count, VkShaderStageFlags shaderStage, const VkSampler* immutableSampler = nullptr);

            void AddPushConstant(VkShaderStageFlags shaderStage, std::uint32_t size, std::uint32_t offset);

            void CreateSet() ADH_NOEXCEPT;

            VkPipelineLayout GetPipelineLayout() noexcept;

            const VkPipelineLayout GetPipelineLayout() const noexcept;

            Array<VkDescriptorSetLayout>& GetSetLayout() noexcept;

            const Array<VkDescriptorSetLayout>& GetSetLayout() const noexcept;

            VkPushConstantRange* GetPushConstant() noexcept;

            const VkPushConstantRange* GetPushConstant() const noexcept;

            operator VkPipelineLayout();

            operator const VkPipelineLayout() const;

          private:
            void MoveConstruct(PipelineLayout&& rhs) noexcept;

            void Clear() noexcept;

          private:
            VkPipelineLayout m_PipelineLayout;
            Array<VkDescriptorSetLayoutBinding> m_LayoutBindings;
            Array<VkDescriptorSetLayout> m_DescriptorSetLayout;
            Array<VkPushConstantRange> m_PushConstants;
        };
    } // namespace vk
} // namespace adh
