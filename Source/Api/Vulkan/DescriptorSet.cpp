#include "DescriptorSet.hpp"
#include "Context.hpp"
#include "Initializers.hpp"

namespace adh {
    namespace vk {
        DescriptorSet::DescriptorSet() noexcept : m_Pool{ VK_NULL_HANDLE },
                                                  m_PipelineLayout{ VK_NULL_HANDLE },
                                                  m_BindPoint{},
                                                  m_SwapChainImageViews{} {
        }

        DescriptorSet::DescriptorSet(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, std::uint32_t swapchainImageViews) {
            Initialize(bindPoint, pipelineLayout, swapchainImageViews);
        }

        DescriptorSet::DescriptorSet(DescriptorSet&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        DescriptorSet& DescriptorSet::operator=(DescriptorSet&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));
            return *this;
        }

        DescriptorSet::~DescriptorSet() {
            Clear();
        }

        void DescriptorSet::Initialize(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, std::uint32_t swapchainImageViews) {
            m_BindPoint           = bindPoint;
            m_PipelineLayout      = pipelineLayout;
            m_SwapChainImageViews = swapchainImageViews;
        }

        void DescriptorSet::Create(const Array<VkDescriptorSetLayout>& descriptorSetLayout) {
            ADH_THROW(!m_PoolSizes.IsEmpty(), "No allocated pools! Call AddPool() before Create()!");
            CreatePool(descriptorSetLayout.GetSize() * m_SwapChainImageViews);
            AllocateDescriptors(descriptorSetLayout);
        }

        void DescriptorSet::AddPool(VkDescriptorType type, std::uint32_t count) {
            ADH_THROW(m_PipelineLayout, "Null pipeline layout handle! Call Initialize() first!");
            m_PoolSizes.EmplaceBack(VkDescriptorPoolSize{ static_cast<VkDescriptorType>(type), count * m_SwapChainImageViews });
        }

        void DescriptorSet::Update(
            VkBuffer buffer,
            VkDeviceSize offset,
            VkDeviceSize range,
            std::uint32_t dstSetIndex,
            std::uint32_t dstBinding,
            std::uint32_t arrayElement,
            std::uint32_t arrayCount,
            VkDescriptorType type) {
            ADH_THROW(!m_DescriptorSets.IsEmpty(), "Empty descriptor sets! Call Create() before Update()!");
            auto info{ initializers::DescriptorBufferInfo(buffer, offset, range) };
            VkWriteDescriptorSet* writeSets{ static_cast<VkWriteDescriptorSet*>(ADH_STACK_ALLOC(m_SwapChainImageViews * sizeof(VkWriteDescriptorSet))) };
            ADH_THROW(writeSets, "Failed to allocate memory for writing descriptor sets!");
            for (std::size_t i{}; i != m_SwapChainImageViews; ++i) {
                writeSets[i] = initializers::WriteDescriptorSet(
                    GetSet(dstSetIndex, i),
                    dstBinding,
                    arrayElement,
                    arrayCount,
                    static_cast<VkDescriptorType>(type),
                    nullptr,
                    &info,
                    nullptr);
            }
            vkUpdateDescriptorSets(vk::Context::Get()->GetDevice(), m_SwapChainImageViews, writeSets, 0, nullptr);
        }

        void DescriptorSet::Update(
            VkDescriptorBufferInfo bufferInfo,
            std::uint32_t dstSetIndex,
            std::uint32_t dstBinding,
            std::uint32_t arrayElement,
            std::uint32_t arrayCount,
            VkDescriptorType type) {
            ADH_THROW(!m_DescriptorSets.IsEmpty(), "Empty descriptor sets! Call Create() before Update()!");
            VkWriteDescriptorSet* writeSets{ static_cast<VkWriteDescriptorSet*>(ADH_STACK_ALLOC(m_SwapChainImageViews * sizeof(VkWriteDescriptorSet))) };
            ADH_THROW(writeSets, "Failed to allocate memory for writing descriptor sets!");

            for (std::size_t i{}; i != m_SwapChainImageViews; ++i) {
                writeSets[i] = initializers::WriteDescriptorSet(
                    GetSet(dstSetIndex, i),
                    dstBinding,
                    arrayElement,
                    arrayCount,
                    static_cast<VkDescriptorType>(type),
                    nullptr,
                    &bufferInfo,
                    nullptr);
            }
            vkUpdateDescriptorSets(vk::Context::Get()->GetDevice(), m_SwapChainImageViews, writeSets, 0, nullptr);
        }

        void DescriptorSet::Update(
            const VkDescriptorBufferInfo* bufferInfo,
            std::uint32_t dstSetIndex,
            std::uint32_t dstBinding,
            std::uint32_t arrayElement,
            std::uint32_t arrayCount,
            VkDescriptorType type) {
            ADH_THROW(!m_DescriptorSets.IsEmpty(), "Empty descriptor sets! Call Create() before Update()!");
            VkWriteDescriptorSet* writeSets{ static_cast<VkWriteDescriptorSet*>(ADH_STACK_ALLOC(m_SwapChainImageViews * sizeof(VkWriteDescriptorSet))) };
            ADH_THROW(writeSets, "Failed to allocate memory for writing descriptor sets!");
            for (std::size_t i{}; i != m_SwapChainImageViews; ++i) {
                writeSets[i] = initializers::WriteDescriptorSet(
                    GetSet(dstSetIndex, i),
                    dstBinding,
                    arrayElement,
                    arrayCount,
                    static_cast<VkDescriptorType>(type),
                    nullptr,
                    &bufferInfo[i],
                    nullptr);
            }
            vkUpdateDescriptorSets(vk::Context::Get()->GetDevice(), m_SwapChainImageViews, writeSets, 0, nullptr);
        }

        void DescriptorSet::Update(
            VkSampler sampler,
            VkImageView imageView,
            VkImageLayout imageLayout,
            std::uint32_t dstSetIndex,
            std::uint32_t dstBinding,
            std::uint32_t arrayElement,
            std::uint32_t arrayCount,
            VkDescriptorType type) {
            ADH_THROW(!m_DescriptorSets.IsEmpty(), "Empty descriptor sets! Call Create() before Update()!");
            auto info{ initializers::DescriptorImageInfo(sampler, imageView, static_cast<VkImageLayout>(imageLayout)) };
            VkWriteDescriptorSet* writeSets{ static_cast<VkWriteDescriptorSet*>(ADH_STACK_ALLOC(m_SwapChainImageViews * sizeof(VkWriteDescriptorSet))) };
            ADH_THROW(writeSets, "Failed to allocate memory for writing descriptor sets!");
            for (std::size_t i{}; i != m_SwapChainImageViews; ++i) {
                writeSets[i] = initializers::WriteDescriptorSet(
                    GetSet(dstSetIndex, i),
                    dstBinding,
                    arrayElement,
                    arrayCount,
                    static_cast<VkDescriptorType>(type),
                    &info,
                    nullptr,
                    nullptr);
            }
            vkUpdateDescriptorSets(vk::Context::Get()->GetDevice(), m_SwapChainImageViews, writeSets, 0, nullptr);
        }

        void DescriptorSet::Update(
            VkDescriptorImageInfo imageInfo,
            std::uint32_t dstSetIndex,
            std::uint32_t dstBinding,
            std::uint32_t arrayElement,
            std::uint32_t arrayCount,
            VkDescriptorType type) {
            ADH_THROW(!m_DescriptorSets.IsEmpty(), "Empty descriptor sets! Call Create() before Update()!");
            VkWriteDescriptorSet* writeSets{ static_cast<VkWriteDescriptorSet*>(ADH_STACK_ALLOC(m_SwapChainImageViews * sizeof(VkWriteDescriptorSet))) };
            ADH_THROW(writeSets, "Failed to allocate memory for writing descriptor sets!");
            for (std::size_t i{}; i != m_SwapChainImageViews; ++i) {
                writeSets[i] = initializers::WriteDescriptorSet(
                    GetSet(dstSetIndex, i),
                    dstBinding,
                    arrayElement,
                    arrayCount,
                    static_cast<VkDescriptorType>(type),
                    &imageInfo,
                    nullptr,
                    nullptr);
            }
            vkUpdateDescriptorSets(vk::Context::Get()->GetDevice(), m_SwapChainImageViews, writeSets, 0, nullptr);
        }

        void DescriptorSet::Update(
            const VkDescriptorImageInfo* imageInfo,
            std::uint32_t dstSetIndex,
            std::uint32_t dstBinding,
            std::uint32_t arrayElement,
            std::uint32_t arrayCount,
            VkDescriptorType type) {
            ADH_THROW(!m_DescriptorSets.IsEmpty(), "Empty descriptor sets! Call Create() before Update()!");
            VkWriteDescriptorSet* writeSets{ static_cast<VkWriteDescriptorSet*>(ADH_STACK_ALLOC(m_SwapChainImageViews * sizeof(VkWriteDescriptorSet))) };
            ADH_THROW(writeSets, "Failed to allocate memory for writing descriptor sets!");
            for (std::size_t i{}; i != m_SwapChainImageViews; ++i) {
                writeSets[i] = initializers::WriteDescriptorSet(
                    GetSet(dstSetIndex, i),
                    dstBinding,
                    arrayElement,
                    arrayCount,
                    static_cast<VkDescriptorType>(type),
                    &imageInfo[i],
                    nullptr,
                    nullptr);
            }
            vkUpdateDescriptorSets(vk::Context::Get()->GetDevice(), m_SwapChainImageViews, writeSets, 0, nullptr);
        }

        void DescriptorSet::Bind(VkCommandBuffer commandBuffer, std::uint32_t imageIndex) ADH_NOEXCEPT {
            auto count{ m_DescriptorSets.GetSize() / m_SwapChainImageViews };
            VkDescriptorSet* sets{ static_cast<VkDescriptorSet*>(ADH_STACK_ALLOC(count * sizeof(VkDescriptorSet))) };
            ADH_THROW(sets, "Failed to allocate memory for descriptor sets!");

            for (std::uint32_t i{}; i != count; ++i) {
                sets[i] = GetSet(i, imageIndex);
            }
            vkCmdBindDescriptorSets(commandBuffer, m_BindPoint, m_PipelineLayout, 0u, count, sets, 0u, nullptr); // TODO: Support for dynamic descriptor sets
        }

        void DescriptorSet::Destroy() noexcept {
            Clear();
        }

        VkDescriptorSet& DescriptorSet::GetSet(std::uint32_t setIndex, std::uint32_t imageIndex) noexcept {
            return m_DescriptorSets[imageIndex + static_cast<std::size_t>(m_SwapChainImageViews) * setIndex];
        }

        const VkDescriptorSet& DescriptorSet::GetSet(std::uint32_t setIndex, std::uint32_t imageIndex) const noexcept {
            return m_DescriptorSets[imageIndex + static_cast<std::size_t>(m_SwapChainImageViews) * setIndex];
        }

        void DescriptorSet::CreatePool(std::uint32_t maxSize) ADH_NOEXCEPT {
            auto info{ initializers::DescriptorPoolCreateInfo(maxSize, static_cast<std::uint32_t>(m_PoolSizes.GetSize()), m_PoolSizes.GetData()) };
            ADH_THROW(vkCreateDescriptorPool(vk::Context::Get()->GetDevice(), &info, nullptr, &m_Pool) == VK_SUCCESS,
                      "Failed to create descriptor pool!");
        }

        void DescriptorSet::AllocateDescriptors(const Array<VkDescriptorSetLayout>& descriptorSetLayout) {
            Array<VkDescriptorSetLayout> setLayouts;
            setLayouts.Reserve(descriptorSetLayout.GetSize() * m_SwapChainImageViews);
            for (std::size_t i{}; i != descriptorSetLayout.GetSize() * m_SwapChainImageViews; ++i) {
                setLayouts.EmplaceBack(descriptorSetLayout[i / m_SwapChainImageViews]);
            }
            auto info{ initializers::DescriptorSetAllocateInfo(m_Pool, static_cast<std::uint32_t>(setLayouts.GetSize()), setLayouts.GetData()) };
            m_DescriptorSets.Resize(setLayouts.GetSize());
            ADH_THROW(vkAllocateDescriptorSets(vk::Context::Get()->GetDevice(), &info, m_DescriptorSets.GetData()) == VK_SUCCESS,
                      "Failed to allocate descriptor sets!");
        }

        void DescriptorSet::MoveConstruct(DescriptorSet&& rhs) noexcept {
            m_Pool                = rhs.m_Pool;
            rhs.m_Pool            = VK_NULL_HANDLE;
            m_PoolSizes           = Move(rhs.m_PoolSizes);
            m_DescriptorSets      = Move(rhs.m_DescriptorSets);
            m_SwapChainImageViews = rhs.m_SwapChainImageViews;
        }

        void DescriptorSet::Clear() noexcept {
            auto device{ Context::Get()->GetDevice() };
            if (m_Pool != VK_NULL_HANDLE) {
                vkDestroyDescriptorPool(device, m_Pool, nullptr);
                m_Pool = VK_NULL_HANDLE;
            }
            m_PoolSizes.Clear();
            m_DescriptorSets.Clear();
        }

        VkDescriptorPool DescriptorSet::GetPool() noexcept {
            return m_Pool;
        }

        const VkDescriptorPool DescriptorSet::GetPool() const noexcept {
            return m_Pool;
        }
    } // namespace vk
} // namespace adh
