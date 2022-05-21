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

#include "Editor.hpp"
#include "vulkan/vulkan_core.h"
#include <Scene/Scene.hpp>
#include <Vulkan/Context.hpp>
#include <Vulkan/Initializers.hpp>
#include <Vulkan/Memory.hpp>
#include <Vulkan/Sampler.hpp>
#include <Vulkan/Tools.hpp>

namespace adh {
    Editor::Editor(Editor&& rhs) noexcept {
        MoveConstruct(Move(rhs));
    }

    Editor& Editor::operator=(Editor&& rhs) noexcept {
        Clear();
        MoveConstruct(Move(rhs));
        return *this;
    }

    Editor::~Editor() {
        Clear();
    }

    void Editor::CreateRenderPass(const vk::Attachment& attachment,
                                  const vk::Subpass& subpass,
                                  const VkRect2D& renderArea,
                                  const Array<VkClearValue>& clearValues) {
        m_RenderPass.Create(attachment, subpass, renderArea, clearValues);
    }

    void Editor::AddGraphicsPipeline(const vk::Shader& shader,
                                     const vk::VertexLayout& vertexLayout,
                                     const vk::PipelineLayout& layout,
                                     VkPrimitiveTopology topology,
                                     VkCullModeFlagBits cullMode,
                                     VkFrontFace frontFace,
                                     VkSampleCountFlagBits rasterizationSamples,
                                     VkBool32 sampleShadingEnable,
                                     float minSampleShading,
                                     VkBool32 enableBlending) {
        m_GraphicsPipelines.EmplaceBack().Create(shader, vertexLayout, layout, m_RenderPass, topology, cullMode,
                                                 frontFace, rasterizationSamples, sampleShadingEnable, minSampleShading, enableBlending);
    }

    void Editor::AddGraphicsPipeline(VkGraphicsPipelineCreateInfo pipelineCreateInfo) {
        m_GraphicsPipelines.EmplaceBack().Create(pipelineCreateInfo);
    }

    void Editor::CreateFramebuffers(vk::Swapchain& swapchain) {
        m_Images.Resize(swapchain.GetImageViewCount() * 2);
        m_Framebuffers.Resize(swapchain.GetImageViewCount() * 2);
        for (std::uint32_t i{}; i != swapchain.GetImageViewCount() * 2; ++i) {
            m_Images[i].Create(
                { swapchain.GetExtent().width, swapchain.GetExtent().height, 1u },
                VK_FORMAT_B8G8R8A8_UNORM,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_TYPE_2D,
                VkImageCreateFlagBits(0),
                1,
                1u,
                VK_SAMPLE_COUNT_1_BIT,
                VkImageUsageFlagBits(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT),
                VK_IMAGE_ASPECT_COLOR_BIT,
                VK_IMAGE_VIEW_TYPE_2D,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                VK_SHARING_MODE_EXCLUSIVE);

            vk::TransferImageLayout(
                m_Images[i],
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_IMAGE_ASPECT_COLOR_BIT);

            VkImageView viewAttachments[]{
                m_Images[i].GetImageView(),
                swapchain.GetDepthBuffer().GetImageView()
            };

            auto info{ vk::initializers::FramebufferCreateInfo(m_RenderPass, std::size(viewAttachments), viewAttachments, swapchain.GetExtent(), 1u) };
            ADH_THROW(vkCreateFramebuffer(vk::Context::Get()->GetDevice(), &info, nullptr, &m_Framebuffers[i]) == VK_SUCCESS,
                      "Failed to create frame buffers!");
        }
        m_RenderPass.UpdateRenderArea({ {}, swapchain.GetExtent() });
        m_SwapchainImageCount = swapchain.GetImageViewCount();
    }

    void Editor::InitOverlay(vk::Swapchain& swapchain, Scene& scene, vk::Sampler& sampler, float aspecRatioWidth, float aspecRatioHeight) {
        m_Overlay.Create(
            swapchain.GetExtent().width,
            swapchain.GetExtent().height,
            aspecRatioWidth,
            aspecRatioHeight,
            m_RenderPass,
            vk::Context::Get()->GetQueue(vk::DeviceQueues::Family::eGraphics).queue,
            swapchain.GetImageViewCount(),
            &scene);
        {
            Array<VkImageView> views;
            for (std::size_t i{}; i != m_SwapchainImageCount; ++i) {
                views.EmplaceBack(m_Images[i].GetImageView());
            }
            m_Overlay.AddTexture("Scene Viewport", views.GetData(), sampler);
        }
        {
            Array<VkImageView> views;
            for (std::size_t i{ m_SwapchainImageCount }; i != m_SwapchainImageCount * 2; ++i) {
                views.EmplaceBack(m_Images[i].GetImageView());
            }
            m_Overlay.AddTexture("Game Viewport", views.GetData(), sampler);
        }
    }

    void Editor::UpdateOverlay(VkExtent2D extent, vk::Sampler& sampler) {
        m_Overlay.SetUpDisplaySize(static_cast<float>(extent.width), static_cast<float>(extent.height));
        {
            Array<VkImageView> views;
            for (std::size_t i{}; i != m_SwapchainImageCount; ++i) {
                views.EmplaceBack(m_Images[i].GetImageView());
            }
            m_Overlay.UpdateTexture("Scene Viewport", views.GetData(), sampler);
        }
        {
            Array<VkImageView> views;
            for (std::size_t i{ m_SwapchainImageCount }; i != m_SwapchainImageCount * 2; ++i) {
                views.EmplaceBack(m_Images[i].GetImageView());
            }
            m_Overlay.UpdateTexture("Game Viewport", views.GetData(), sampler);
        }
    }

    void Editor::OnUpdate(Scene* scene, float deltaTime, bool drawEditor) {
        m_Overlay.OnUpdate(scene, deltaTime, drawEditor);
    }

    void Editor::Draw(VkCommandBuffer cmd, std::uint32_t imageIndex, bool* maximizeOnPlay, bool* play, bool* pause, float* floats[]) {
        m_Overlay.Draw(cmd, imageIndex, maximizeOnPlay, play, pause, floats);
    }

    bool Editor::GetKeyDown(std::uint64_t keycode) noexcept {
        return m_Overlay.GetKeyDown(keycode);
    }

    float Editor::GetSelectedAspectRatio() const noexcept {
        return m_Overlay.GetSelectedAspectRatio();
    }

    float Editor::GetSelectedAspectRatioWidth() const noexcept {
        return m_Overlay.GetSelectedAspectRatioWidth();
    }

    float Editor::GetSelectedAspectRatioHeight() const noexcept {
        return m_Overlay.GetSelectedAspectRatioHeight();
    }

    void Editor::Recreate(vk::Swapchain& swapchain) {
        for (std::uint32_t i{}; i != swapchain.GetImageViewCount() * 2; ++i) {
            m_Images[i].Destroy();
            vkDestroyFramebuffer(vk::Context::Get()->GetDevice(), m_Framebuffers[i], nullptr);
        }
        CreateFramebuffers(swapchain);
        m_RenderPass.UpdateRenderArea({ {}, swapchain.GetExtent() });
    }

    void Editor::BeginRenderPass(VkCommandBuffer cmd, std::uint32_t imageIndex, std::uint32_t viewportIndex) {
        m_RenderPass.Begin(cmd, m_Framebuffers[imageIndex + (m_SwapchainImageCount * viewportIndex)]);
    }

    void Editor::BindGraphicsPipeline(VkCommandBuffer cmd, std::uint32_t index) {
        m_GraphicsPipelines[index].Bind(cmd);
    }

    void Editor::EndRenderPass(VkCommandBuffer cmd) {
        m_RenderPass.End(cmd);
    }

    void Editor::Destroy() noexcept {
        Clear();
    }

    void Editor::MoveConstruct(Editor&& rhs) noexcept {
        m_Images            = Move(rhs.m_Images);
        m_Framebuffers      = Move(rhs.m_Framebuffers);
        m_RenderPass        = Move(rhs.m_RenderPass);
        m_GraphicsPipelines = Move(rhs.m_GraphicsPipelines);
    }

    void Editor::Clear() noexcept {
        m_Images.Clear();
        for (std::uint32_t i{}; i != m_Framebuffers.GetSize(); ++i) {
            vkDestroyFramebuffer(vk::Context::Get()->GetDevice(), m_Framebuffers[i], nullptr);
        }
        m_Framebuffers.Clear();
        m_RenderPass.Destroy();
        m_GraphicsPipelines.Clear();
    }

} // namespace adh
