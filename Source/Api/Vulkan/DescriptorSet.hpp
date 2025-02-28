#pragma once
#include <Std/Array.hpp>
#include <vulkan/vulkan.h>

#include <Vulkan/Context.hpp>

#include <Std/Utility.hpp>
#include <Utility.hpp>
#include <Vulkan/Initializers.hpp>

namespace adh {
    namespace vk {
        class TextureDescriptors {
          public:
            static void Initialize(
                uint32_t descriptorIndex,
                uint32_t bindingIndex) {

                mDescriptorIndex = descriptorIndex;
                mBindingIndex    = bindingIndex;

                VkDescriptorSetLayoutBinding uboLayoutBinding{};
                uboLayoutBinding.binding            = bindingIndex;
                uboLayoutBinding.descriptorCount    = 1;
                uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                uboLayoutBinding.pImmutableSamplers = nullptr;
                uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

                VkDescriptorSetLayoutCreateInfo layoutInfo{};
                layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                layoutInfo.bindingCount = 1;
                layoutInfo.pBindings    = &uboLayoutBinding;

                ADH_THROW(vkCreateDescriptorSetLayout(Context::Get()->GetDevice(), &layoutInfo, nullptr, &setLayout) == VK_SUCCESS,
                          "Failed to create set layout!");

                VkDescriptorPoolSize poolSize{};
                poolSize.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                poolSize.descriptorCount = 1000;

                VkDescriptorPoolCreateInfo info{};
                info.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
                info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                info.maxSets       = 1000;
                info.poolSizeCount = 1;
                info.pPoolSizes    = &poolSize;

                ADH_THROW(vkCreateDescriptorPool(Context::Get()->GetDevice(), &info, nullptr, &mPool) == VK_SUCCESS,
                          "Failed to create descriptor pool!");
            }

            static uint32_t GetDescriptorID(VkDescriptorImageInfo imageInfo) {
                VkDescriptorSetAllocateInfo info{};
                info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                info.descriptorPool     = mPool;
                info.descriptorSetCount = 1;
                info.pSetLayouts        = &setLayout;

                auto& set = mDescriptorSets.EmplaceBack();

                ADH_THROW(vkAllocateDescriptorSets(vk::Context::Get()->GetDevice(), &info, &set) == VK_SUCCESS,
                          "Failed to allocate descriptor sets!");

                VkWriteDescriptorSet writeSets = initializers::WriteDescriptorSet(
                    set,
                    mBindingIndex,
                    0,
                    1,
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    &imageInfo,
                    nullptr,
                    nullptr);

                vkUpdateDescriptorSets(Context::Get()->GetDevice(), 1, &writeSets, 0, nullptr);

                return mDescriptorSets.GetSize() - 1;
            }

            static VkDescriptorSet GetDescriptor(uint32_t index) {
                return mDescriptorSets[index];
            }

            static void FreeDescriptor(uint32_t id) {
                clearDescriptors.EmplaceBack(id);
            }

            static void Flush() {
                if (!clearDescriptors.IsEmpty()) {
                    for (int i{}; i != clearDescriptors.GetSize(); ++i) {
                        vkFreeDescriptorSets(Context::Get()->GetDevice(), mPool, 1, &mDescriptorSets[clearDescriptors[i]]);
                    }
                    clearDescriptors.Clear();
                }
            }

            static void CleanUp() {
                auto device{ Context::Get()->GetDevice() };
                vkDestroyDescriptorSetLayout(device, setLayout, nullptr);
                vkDestroyDescriptorPool(device, mPool, nullptr);
            }

          private:
            inline static VkDescriptorPool mPool;
            inline static VkDescriptorSetLayout setLayout;
            inline static Array<VkDescriptorSet> mDescriptorSets;
            inline static uint32_t mDescriptorIndex;
            inline static uint32_t mBindingIndex;

            inline static Array<uint32_t> clearDescriptors;
        };
    } // namespace vk
} // namespace adh

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

          public:
            VkDescriptorPool m_Pool;
            VkPipelineLayout m_PipelineLayout;
            VkPipelineBindPoint m_BindPoint;
            std::uint32_t m_SwapChainImageViews;
            Array<VkDescriptorPoolSize> m_PoolSizes;
            Array<VkDescriptorSet> m_DescriptorSets;
        };
    } // namespace vk
} // namespace adh
