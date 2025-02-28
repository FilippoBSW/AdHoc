#include "Subpass.hpp"
#include "Initializers.hpp"

namespace adh {
    namespace vk {
        void Subpass::AddDescription(VkPipelineBindPoint bindPoint, const Attachment& attachment) {
            m_Descriptions.EmplaceBack(
                initializers::SubpassDescription(
                    static_cast<VkPipelineBindPoint>(bindPoint), // Bind point
                    attachment.GetColorReferences().GetSize(),   // color count
                    attachment.GetColorReferences().GetData(),   // color ptr
                    attachment.GetResolveReferences().GetData(), // resolve ptr
                    attachment.GetDepthReferences().GetData(),   // depth ptr
                    attachment.GetInputReferences().GetSize(),   // input count
                    attachment.GetInputReferences().GetData())); // input ptr
        }

        void Subpass::AddDependencies(
            std::uint32_t srcSubpass,
            std::uint32_t dstSubpass,
            VkPipelineStageFlagBits srcStageMask,
            VkPipelineStageFlagBits dstStageMask,
            VkAccessFlagBits srcAccessMask,
            VkAccessFlagBits dstAccessMask,
            VkDependencyFlags dependencyFlags) {
            m_Dependencies.EmplaceBack(
                initializers::SubpassDependency(
                    srcSubpass,
                    dstSubpass,
                    static_cast<VkPipelineStageFlags>(srcStageMask),
                    static_cast<VkPipelineStageFlags>(dstStageMask),
                    static_cast<VkAccessFlags>(srcAccessMask),
                    static_cast<VkAccessFlags>(dstAccessMask),
                    dependencyFlags));
        }

        Array<VkSubpassDescription>& Subpass::GetDescriptions() noexcept {
            return m_Descriptions;
        }

        const Array<VkSubpassDescription>& Subpass::GetDescriptions() const noexcept {
            return m_Descriptions;
        }

        Array<VkSubpassDependency>& Subpass::GetDependencies() noexcept {
            return m_Dependencies;
        }

        const Array<VkSubpassDependency>& Subpass::GetDependencies() const noexcept {
            return m_Dependencies;
        }
    } // namespace vk
} // namespace adh
