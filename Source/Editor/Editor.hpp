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

#include <Std/Array.hpp>
#include <Std/StaticArray.hpp>
#include <Vulkan/Attachments.hpp>
#include <Vulkan/GraphicsPipeline.hpp>
#include <Vulkan/Image.hpp>
#include <Vulkan/PipelineLayout.hpp>
#include <Vulkan/RenderPass.hpp>
#include <Vulkan/Shader.hpp>
#include <Vulkan/Subpass.hpp>
#include <Vulkan/Swapchain.hpp>
#include <Vulkan/VertexLayout.hpp>

#include "UIOverlay/UIOverlay.hpp"

namespace adh {
    class Editor {
      public:
        Editor() noexcept = default;

        Editor(const Editor& rhs) = delete;

        Editor& operator=(const Editor& rhs) = delete;

        Editor(Editor&& rhs) noexcept;

        Editor& operator=(Editor&& rhs) noexcept;

        ~Editor();

        void CreateRenderPass(const vk::Attachment& attachment,
                              const vk::Subpass& subpass,
                              const VkRect2D& renderArea,
                              const Array<VkClearValue>& clearValues);

        void AddGraphicsPipeline(const vk::Shader& shader,
                                 const vk::VertexLayout& vertexLayout,
                                 const vk::PipelineLayout& layout,
                                 VkPrimitiveTopology topology,
                                 VkCullModeFlagBits cullMode,
                                 VkFrontFace frontFace,
                                 VkBool32 enableBlending);

        void AddGraphicsPipeline(VkGraphicsPipelineCreateInfo pipelineCreateInfo);

        void CreateFramebuffers(vk::Swapchain& swapchain);

        void InitOverlay(vk::Swapchain& swapchain, Scene& scene, vk::Sampler& sampler, float windowWidth, float windowHeight);

        void UpdateOverlay(VkExtent2D extent, vk::Sampler& sampler);

        void OnUpdate(Scene* scene, float deltaTime, bool drawEditor);

        void Draw(VkCommandBuffer cmd, std::uint32_t imageIndex, bool* maximizeOnPlay, bool* play, bool* pause);

        void Recreate(vk::Swapchain& swapchain);

        bool GetKeyDown(std::uint64_t keycode) noexcept;

        float GetSelectedAspectRatio() const noexcept;

        float GetSelectedAspectRatioWidth() const noexcept;

        float GetSelectedAspectRatioHeight() const noexcept;

        void BeginRenderPass(VkCommandBuffer cmd, std::uint32_t imageIndex, std::uint32_t viewportIndex);

        void BindGraphicsPipeline(VkCommandBuffer cmd, std::uint32_t index = 0u);

        void EndRenderPass(VkCommandBuffer cmd);

        void Destroy() noexcept;

      private:
        void MoveConstruct(Editor&& rhs) noexcept;

        void Clear() noexcept;

      private:
        Array<vk::Image> m_Images;
        Array<VkFramebuffer> m_Framebuffers;
        vk::RenderPass m_RenderPass;
        Array<vk::GraphicsPipeline> m_GraphicsPipelines;
        UIOverlay m_Overlay;
        std::uint32_t m_SwapchainImageCount;
    };
} // namespace adh
