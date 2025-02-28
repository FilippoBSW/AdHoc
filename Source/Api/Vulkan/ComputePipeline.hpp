#pragma once
#include <Utility.hpp>

#include <iostream>
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        class ComputePipeline {
          public:
            ComputePipeline() noexcept;

            ComputePipeline(
                VkPipelineLayout layout,
                VkPipelineShaderStageCreateInfo stage,
                std::uint32_t localSizeX,
                std::uint32_t elementSizeX,
                std::uint32_t localSizeY,
                std::uint32_t elementSizeY,
                std::uint32_t localSizeZ,
                std::uint32_t elementSizeZ) ADH_NOEXCEPT;

            ComputePipeline(const ComputePipeline& rhs) = delete;

            ComputePipeline& operator=(const ComputePipeline& rhs) = delete;

            ComputePipeline(ComputePipeline&& rhs) noexcept;

            ComputePipeline& operator=(ComputePipeline&& rhs) noexcept;

            ~ComputePipeline();

            void Create(
                VkPipelineLayout layout,
                VkPipelineShaderStageCreateInfo stage,
                std::uint32_t localSizeX,
                std::uint32_t elementSizeX,
                std::uint32_t localSizeY,
                std::uint32_t elementSizeY,
                std::uint32_t localSizeZ,
                std::uint32_t elementSizeZ) ADH_NOEXCEPT;

            void Bind(VkCommandBuffer commandBuffer) noexcept;

            void Dispatch(VkCommandBuffer commandBuffer) noexcept;

            void Destroy() noexcept;

            operator VkPipeline() noexcept;

            operator const VkPipeline() const noexcept;

          private:
            void MoveConstruct(ComputePipeline&& rhs) noexcept;

            void Clear() noexcept;

          private:
            VkPipeline m_Pipeline;

            union {
                std::uint32_t m_Groups[3];
                struct {
                    std::uint32_t x;
                    std::uint32_t y;
                    std::uint32_t z;
                };
            };
        };
    } // namespace vk
} // namespace adh
