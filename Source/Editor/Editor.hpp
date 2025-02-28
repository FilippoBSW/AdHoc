#pragma once

#include <Std/Array.hpp>
#include <Std/StaticArray.hpp>
#include <Vulkan/Attachments.hpp>
#include <Vulkan/Framebuffer.hpp>
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
                                 VkSampleCountFlagBits rasterizationSamples,
                                 VkBool32 sampleShadingEnable,
                                 float minSampleShading,
                                 VkBool32 enableBlending);

        void AddGraphicsPipeline(VkGraphicsPipelineCreateInfo pipelineCreateInfo);

        void CreateFramebuffers(vk::Swapchain& swapchain);

        void InitOverlay(vk::Swapchain& swapchain, Scene& scene, vk::Sampler& sampler, float windowWidth, float windowHeight);

        void UpdateOverlay(VkExtent2D extent, vk::Sampler& sampler);

        void OnUpdate(Scene* scene, float deltaTime, bool drawEditor);

        void Draw(VkCommandBuffer cmd, std::uint32_t imageIndex, bool* maximizeOnPlay, bool* play, bool* pause, bool* fpsLimit, float* floats[], Vector3D& sunPosition);

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
        Array<vk::Framebuffer> m_Framebuffers;
        // Array<VkFramebuffer> m_Framebuffers;
        vk::RenderPass m_RenderPass;
        Array<vk::GraphicsPipeline> m_GraphicsPipelines;
        UIOverlay m_Overlay;
        std::uint32_t m_SwapchainImageCount;
    };
} // namespace adh
