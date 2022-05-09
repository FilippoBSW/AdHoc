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
