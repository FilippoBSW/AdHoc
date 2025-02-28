#include "GraphicsPipeline.hpp"
#include "Context.hpp"
#include <Std/StaticArray.hpp>

namespace adh {
    namespace vk {
        GraphicsPipeline::GraphicsPipeline() noexcept : m_Pipeline{ VK_NULL_HANDLE } {
        }

        GraphicsPipeline::GraphicsPipeline(
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
            VkBool32 enableBlending) {
            Create(shader, vertexLayout, layout, renderPass, topology, cullMode, frontFace, rasterizationSamples, sampleShadingEnable, minSampleShading, enableBlending);
        }

        GraphicsPipeline::GraphicsPipeline(VkGraphicsPipelineCreateInfo createInfo) {
            Create(createInfo);
        }

        GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));

            return *this;
        }

        GraphicsPipeline::~GraphicsPipeline() {
            Clear();
        }

        void GraphicsPipeline::Create(
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
            VkBool32 enableBlending) {
            auto inputAssembly{ initializers::PipelineInputAssemblyStateCreateInfo(topology) };
            auto viewportState{ initializers::PipelineViewportStateCreateInfo() };
            auto rasterizationState{ initializers::PipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, cullMode, frontFace) };
            auto multisample{ initializers::PipelineMultisampleStateCreateInfo(rasterizationSamples, sampleShadingEnable, minSampleShading) };
            auto depthStencil{ initializers::PipelineDepthStencilStateCreateInfo(VK_COMPARE_OP_LESS_OR_EQUAL) };
            auto colorBlendAttachmentState{ initializers::PipelineColorBlendAttachmentState(static_cast<VkBool32>(enableBlending)) };
            auto colorBlendStateInfo{ initializers::PipelineColorBlendStateCreateInfo(1u, &colorBlendAttachmentState) };

            StaticArray<VkDynamicState, 3u> dynamicStates{
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR,
                VK_DYNAMIC_STATE_DEPTH_BIAS
            };

            auto dynamicState{ initializers::PipelineDynamicStateCreateInfo(dynamicStates.GetData(), dynamicStates.GetSize()) };

            auto graphicsPipelineInfo{
                initializers::GraphicsPipelineCreateInfo(
                    shader.GetSize(),
                    &shader.Get(),
                    &vertexLayout.Get(),
                    &inputAssembly,
                    nullptr,
                    &viewportState,
                    &rasterizationState,
                    &multisample,
                    &depthStencil,
                    &colorBlendStateInfo,
                    &dynamicState,
                    layout,
                    renderPass)
            };

            ADH_THROW(vkCreateGraphicsPipelines(Context::Get()->GetDevice(), VK_NULL_HANDLE, 1u, &graphicsPipelineInfo, nullptr, &m_Pipeline) == VK_SUCCESS,
                      "Failed to create graphics pipeline!");
        }

        void GraphicsPipeline::Create(VkGraphicsPipelineCreateInfo createInfo) {
            ADH_THROW(vkCreateGraphicsPipelines(Context::Get()->GetDevice(), VK_NULL_HANDLE, 1u, &createInfo, nullptr, &m_Pipeline) == VK_SUCCESS,
                      "Failed to create graphics pipeline!");
        }

        void GraphicsPipeline::Bind(VkCommandBuffer commandBuffer) {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
        }

        VkPipeline GraphicsPipeline::Get() noexcept {
            return m_Pipeline;
        }

        const VkPipeline GraphicsPipeline::Get() const noexcept {
            return m_Pipeline;
        }

        void GraphicsPipeline::Destroy() noexcept {
            Clear();
        }

        GraphicsPipeline::operator VkPipeline() noexcept {
            return m_Pipeline;
        }

        GraphicsPipeline::operator const VkPipeline() const noexcept {
            return m_Pipeline;
        }

        void GraphicsPipeline::MoveConstruct(GraphicsPipeline&& rhs) noexcept {
            m_Pipeline     = rhs.m_Pipeline;
            rhs.m_Pipeline = VK_NULL_HANDLE;
        }

        void GraphicsPipeline::Clear() noexcept {
            if (m_Pipeline != VK_NULL_HANDLE) {
                vkDestroyPipeline(Context::Get()->GetDevice(), m_Pipeline, nullptr);
                m_Pipeline = VK_NULL_HANDLE;
            }
        }
    } // namespace vk
} // namespace adh
