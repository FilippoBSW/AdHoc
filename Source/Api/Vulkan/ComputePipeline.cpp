#include "ComputePipeline.hpp"
#include "Context.hpp"
#include "Initializers.hpp"

namespace adh {
    namespace vk {
        ComputePipeline::ComputePipeline() noexcept : m_Pipeline{ VK_NULL_HANDLE },
                                                      m_Groups{} {
        }

        ComputePipeline::ComputePipeline(
            VkPipelineLayout layout,
            VkPipelineShaderStageCreateInfo stage,
            std::uint32_t localSizeX,
            std::uint32_t elementSizeX,
            std::uint32_t localSizeY,
            std::uint32_t elementSizeY,
            std::uint32_t localSizeZ,
            std::uint32_t elementSizeZ) ADH_NOEXCEPT {
            Create(layout, stage, localSizeX, elementSizeX, localSizeY, elementSizeY, localSizeZ, elementSizeZ);
        }

        ComputePipeline::ComputePipeline(ComputePipeline&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        ComputePipeline& ComputePipeline::operator=(ComputePipeline&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));
            return *this;
        }

        ComputePipeline::~ComputePipeline() {
            Clear();
        }

        void ComputePipeline::Create(
            VkPipelineLayout layout,
            VkPipelineShaderStageCreateInfo stage,
            std::uint32_t localSizeX,
            std::uint32_t elementSizeX,
            std::uint32_t localSizeY,
            std::uint32_t elementSizeY,
            std::uint32_t localSizeZ,
            std::uint32_t elementSizeZ) ADH_NOEXCEPT {
            auto info{ initializers::ComputePipelineCreateInfo(layout, stage) };

            ADH_THROW(vkCreateComputePipelines(
                          Context::Get()->GetDevice(),
                          VK_NULL_HANDLE,
                          1u,
                          &info,
                          nullptr,
                          &m_Pipeline) == VK_SUCCESS,
                      "Failed to create compute pipeline!");

            ADH_THROW((localSizeX) && (localSizeY) && (localSizeZ), "Local size must be => 1!");

            x = ((elementSizeX) / localSizeX) + 1u;
            y = ((elementSizeY) / localSizeY) + 1u;
            z = ((elementSizeZ) / localSizeZ) + 1u;
        }

        void ComputePipeline::Bind(VkCommandBuffer commandBuffer) noexcept {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipeline);
        }

        void ComputePipeline::Dispatch(VkCommandBuffer commandBuffer) noexcept {
            vkCmdDispatch(commandBuffer, x, y, z);
        }

        void ComputePipeline::Destroy() noexcept {
            Clear();
        }

        ComputePipeline::operator VkPipeline() noexcept {
            return m_Pipeline;
        }

        ComputePipeline::operator const VkPipeline() const noexcept {
            return m_Pipeline;
        }

        void ComputePipeline::MoveConstruct(ComputePipeline&& rhs) noexcept {
            m_Pipeline = rhs.m_Pipeline;
            x          = rhs.x;
            y          = rhs.y;
            z          = rhs.z;

            rhs.m_Pipeline = VK_NULL_HANDLE;
        }

        void ComputePipeline::Clear() noexcept {
            if (m_Pipeline != VK_NULL_HANDLE) {
                vkDestroyPipeline(Context::Get()->GetDevice(), m_Pipeline, nullptr);
                m_Pipeline = VK_NULL_HANDLE;
            }
        }
    } // namespace vk
} // namespace adh
