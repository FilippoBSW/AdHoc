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
        class DescriptorSet {
          public:
            DescriptorSet() noexcept;

            DescriptorSet(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, std::uint32_t swapchainImageViews);

            DescriptorSet(const DescriptorSet&) = delete;

            DescriptorSet& operator=(const DescriptorSet&) = delete;

            DescriptorSet(DescriptorSet&& rhs) noexcept;

            DescriptorSet& operator=(DescriptorSet&& rhs) noexcept;

            ~DescriptorSet();

            void Initialize(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, std::uint32_t swapchainImageViews);

            void Create(const Array<VkDescriptorSetLayout>& descriptorSetLayout);

            void AddPool(VkDescriptorType type, std::uint32_t count);

            void Update(
                VkBuffer buffer,
                VkDeviceSize offset,
                VkDeviceSize range,
                std::uint32_t dstSetIndex,
                std::uint32_t dstBinding,
                std::uint32_t arrayElement,
                std::uint32_t arrayCount,
                VkDescriptorType type);

            void Update(
                VkDescriptorBufferInfo bufferInfo,
                std::uint32_t dstSetIndex,
                std::uint32_t dstBinding,
                std::uint32_t arrayElement,
                std::uint32_t arrayCount,
                VkDescriptorType type);

            void Update(
                const VkDescriptorBufferInfo* bufferInfo,
                std::uint32_t dstSetIndex,
                std::uint32_t dstBinding,
                std::uint32_t arrayElement,
                std::uint32_t arrayCount,
                VkDescriptorType type);

            void Update(
                VkSampler sampler,
                VkImageView imageView,
                VkImageLayout imageLayout,
                std::uint32_t dstSetIndex,
                std::uint32_t dstBinding,
                std::uint32_t arrayElement,
                std::uint32_t arrayCount,
                VkDescriptorType type);

            void Update( // TODO: array
                VkDescriptorImageInfo imageInfo,
                std::uint32_t dstSetIndex,
                std::uint32_t dstBinding,
                std::uint32_t arrayElement,
                std::uint32_t arrayCount,
                VkDescriptorType type);

            void Update(
                const VkDescriptorImageInfo* imageInfo,
                std::uint32_t dstSetIndex,
                std::uint32_t dstBinding,
                std::uint32_t arrayElement,
                std::uint32_t arrayCount,
                VkDescriptorType type);

            void Bind(VkCommandBuffer commandBuffer, std::uint32_t imageIndex) ADH_NOEXCEPT;

            void Destroy() noexcept;

            VkDescriptorSet& GetSet(std::uint32_t setIndex, std::uint32_t imageIndex) noexcept;

            const VkDescriptorSet& GetSet(std::uint32_t setIndex, std::uint32_t imageIndex) const noexcept;

            VkDescriptorPool GetPool() noexcept;

            const VkDescriptorPool GetPool() const noexcept;

          private:
            void CreatePool(std::uint32_t maxSize) ADH_NOEXCEPT;

            void AllocateDescriptors(const Array<VkDescriptorSetLayout>& descriptorSetLayout);

            void MoveConstruct(DescriptorSet&& rhs) noexcept;

            void Clear() noexcept;

          private:
            VkDescriptorPool m_Pool;
            VkPipelineLayout m_PipelineLayout;
            VkPipelineBindPoint m_BindPoint;
            std::uint32_t m_SwapChainImageViews;
            Array<VkDescriptorPoolSize> m_PoolSizes;
            Array<VkDescriptorSet> m_DescriptorSets;
        };
    } // namespace vk
} // namespace adh
