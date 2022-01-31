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
#include <Event/Event.hpp>
#include <Math/Math.hpp>
#include <Std/Array.hpp>
#include <Vulkan/DescriptorSet.hpp>
#include <Vulkan/GraphicsPipeline.hpp>
#include <Vulkan/Image.hpp>
#include <Vulkan/Initializers.hpp>
#include <Vulkan/PipelineLayout.hpp>
#include <Vulkan/RenderPass.hpp>
#include <Vulkan/Sampler.hpp>
#include <Vulkan/Scissor.hpp>
#include <Vulkan/Texture2D.hpp>
#include <Vulkan/Tools.hpp>
#include <Vulkan/Viewport.hpp>
#include <unordered_map>
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        struct ImGuiBuffers {
            Buffer buffer;
            std::uint32_t count;
            void* mappedPtr;
        };

        class VulkanImGui {
          private:
            using DescriptorIndexMap = std::unordered_map<std::string, std::uint32_t>;

          public:
            static constexpr std::uint32_t maxTextures{ 100u };

          public:
            void Create(const RenderPass& renderPass, VkQueue queue, std::uint32_t imageViewCount);

            [[nodiscard]] void* AddTexture(const std::string& name, VkImageView imageView, const vk::Sampler& sampler);

            [[nodiscard]] void* AddTexture(const std::string& name, const VkImageView* imageView, const vk::Sampler& sampler);

            void UpdateTexture(const std::string& name, VkImageView imageView, const vk::Sampler& sampler);

            void UpdateTexture(const std::string& name, const VkImageView* imageView, const vk::Sampler& sampler);

            void Draw(VkCommandBuffer commandBuffer, std::uint32_t imageIndex);

          private:
            PipelineLayout m_PipelineLayout;
            Array<DescriptorSet> m_DescriptorSet;
            DescriptorIndexMap m_DescriptorIndices;
            GraphicsPipeline m_GraphicsPipeline;
            Texture2D m_FontTexture;
            Sampler m_FontSampler;
            Array<ImGuiBuffers> m_VertexBuffer;
            Array<ImGuiBuffers> m_IndexBuffer;
            vk::Viewport m_Viewport;
            vk::Scissor m_Scissor;
            std::uint32_t m_ImageViewCount;

            struct {
                Vector2D scale;
                Vector2D translate;
            } m_PushConstantData{};
        };
    } // namespace vk
} // namespace adh
