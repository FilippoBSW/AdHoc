#pragma once
#include "PipelineLayout.hpp"
#include "RenderPass.hpp"
#include "Shader.hpp"
#include "VertexLayout.hpp"
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        class GraphicsPipeline {
          public:
            GraphicsPipeline() noexcept;

            GraphicsPipeline(
                const Shader& shader,
                const VertexLayout& vertexLayout,
                const PipelineLayout& layout,
                const RenderPass& renderPass,
                VkPrimitiveTopology topology,
                VkCullModeFlagBits cullMode,
                VkFrontFace frontFace,
                VkSampleCountFlagBits rasterizationSamples,
                VkBool32 sampleShadingEnable,
                float minSampleShading,
                VkBool32 enableBlending);

            GraphicsPipeline(VkGraphicsPipelineCreateInfo createInfo);

            GraphicsPipeline(const GraphicsPipeline& rhs) = delete;

            GraphicsPipeline& operator=(const GraphicsPipeline& rhs) = delete;

            GraphicsPipeline(GraphicsPipeline&& rhs) noexcept;

            GraphicsPipeline& operator=(GraphicsPipeline&& rhs) noexcept;

            ~GraphicsPipeline();

            void Create(
                const Shader& shader,
                const VertexLayout& vertexLayout,
                const PipelineLayout& layout,
                const RenderPass& renderPass,
                VkPrimitiveTopology topology,
                VkCullModeFlagBits cullMode,
                VkFrontFace frontFace,
                VkSampleCountFlagBits rasterizationSamples,
                VkBool32 sampleShadingEnable,
                float minSampleShading,
                VkBool32 enableBlending);

            void Create(VkGraphicsPipelineCreateInfo createInfo);

            void Bind(VkCommandBuffer commandBuffer);

            VkPipeline Get() noexcept;

            const VkPipeline Get() const noexcept;

            void Destroy() noexcept;

            operator VkPipeline() noexcept;

            operator const VkPipeline() const noexcept;

          private:
            void MoveConstruct(GraphicsPipeline&& rhs) noexcept;

            void Clear() noexcept;

          private:
            VkPipeline m_Pipeline;
        };
    } // namespace vk
} // namespace adh
