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

// TODO: ot
// Notes:
// Texture as Components
// Texture lighing

#include <Audio/Audio.hpp>
#include <Editor/Editor.hpp>
#include <Entity/Entity.hpp>
#include <Event/Event.hpp>
#include <Input/Input.hpp>
#include <Math/Math.hpp>
#include <Scene/Components.hpp>
#include <Scene/Scene.hpp>
#include <Std/StaticArray.hpp>
#include <Std/Stopwatch.hpp>
#include <Utility.hpp>
#include <Vulkan/Attachments.hpp>
#include <Vulkan/CommandBuffer.hpp>
#include <Vulkan/CommandPool.hpp>
#include <Vulkan/Context.hpp>
#include <Vulkan/DescriptorSet.hpp>
#include <Vulkan/Framebuffer.hpp>
#include <Vulkan/GraphicsPipeline.hpp>
#include <Vulkan/ImageView.hpp>
#include <Vulkan/IndexBuffer.hpp>
#include <Vulkan/Memory.hpp>
#include <Vulkan/PipelineLayout.hpp>
#include <Vulkan/RenderPass.hpp>
#include <Vulkan/Sampler.hpp>
#include <Vulkan/Scissor.hpp>
#include <Vulkan/Shader.hpp>
#include <Vulkan/Subpass.hpp>
#include <Vulkan/Swapchain.hpp>
#include <Vulkan/Texture2D.hpp>
#include <Vulkan/Tools.hpp>
#include <Vulkan/UniformBuffer.hpp>
#include <Vulkan/VertexBuffer.hpp>
#include <Vulkan/VertexLayout.hpp>
#include <Vulkan/Viewport.hpp>
#include <Window.hpp>

#include <fstream>

#if defined(ADH_IOS)
#    include <UIKit/UIKit.h>
#endif

using namespace adh;
using namespace adh::vk;
using namespace adh::ecs;

#include <Scripting/Script.hpp>
#include <Scripting/ScriptHandler.hpp>

#include <Vertex.hpp>

struct PBR_UBO {
    xmm::Matrix viewProj{ 1.0f };
} viewProjection;
PBR_UBO editorViewProjection;

xmm::Matrix lightSpace{ 1.0f };

const xmm::Matrix lightSpaceBias{
    0.5f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.5f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.0f, 1.0f
};

Vector3D sunPosition{ 6.0f, 20.0f, -10.0f };

struct FragmentData {
    alignas(16) Vector3D ambient;
    alignas(16) Vector3D cameraPosition;
    alignas(4) int shadowPCF;
} fragmentUbo;

struct AspectRatio {
    float viewportWidth;
    float viewportHeight;
    float viewportX;
    float viewportY;

    void CalculateViewport(const VkExtent2D& extent, float aspectWidth, float aspectHeight) noexcept {
        float width    = static_cast<float>(extent.width);
        float height   = static_cast<float>(extent.height);
        viewportWidth  = width;
        viewportHeight = height;
        viewportX      = 0.0f;
        viewportY      = 0.0f;

        if (width * aspectHeight > height * aspectWidth) {
            viewportWidth = height * aspectWidth / aspectHeight;
            viewportX     = (width - viewportWidth) / 2.0f;
        } else if (width * aspectHeight < height * aspectWidth) {
            viewportHeight = width * aspectHeight / aspectWidth;
            viewportY      = (height - viewportHeight) / 2.0f;
        }
    }

    VkViewport GetViewport() const noexcept {
        VkViewport viewport;
        viewport.x        = viewportX;
        viewport.y        = viewportY + viewportHeight;
        viewport.width    = viewportWidth;
        viewport.height   = -viewportHeight;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        return viewport;
    }

    VkRect2D GetRect() const noexcept {
        VkRect2D rect;
        rect.extent = { static_cast<std::uint32_t>(viewportWidth), static_cast<std::uint32_t>(viewportHeight) };
        rect.offset = { static_cast<std::int32_t>(viewportX), static_cast<std::int32_t>(viewportY) };
        return rect;
    }
};

struct ShadowMap2D {
    struct Debug {
        void Create(RenderPass& renderPass, VkDescriptorImageInfo& info) {

            Shader shader("draw_shadowmap.vert", "draw_shadowmap.frag");

            VertexLayout vertexLayout;
            vertexLayout.Create();

            pipelineLayout.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
            pipelineLayout.CreateSet();

            pipelineLayout.Create();

            graphicsPipeline.Create(shader, vertexLayout, pipelineLayout, renderPass,
                                    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                    VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 0.0f, VK_TRUE);

            descriptorSet.Initialize(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 3);
            descriptorSet.AddPool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
            descriptorSet.Create(pipelineLayout.GetSetLayout());

            descriptorSet.Update(
                info,
                0u,                                       // descriptor index
                1u,                                       // binding
                0u,                                       // array element
                1u,                                       // array count
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
            );
        }

        PipelineLayout pipelineLayout;
        DescriptorSet descriptorSet;
        GraphicsPipeline graphicsPipeline;
    };

    void Create(RenderPass& renderPass, Sampler& sampler) {
        Attachment attachment;
        attachment.AddDescription(
            tools::GetSupportedDepthFormat(Context::Get()->GetPhysicalDevice()),
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            Attachment::Type::eDepth);

        Subpass subpass;
        subpass.AddDescription(VK_PIPELINE_BIND_POINT_GRAPHICS, attachment);
        subpass.AddDependencies(
            VK_SUBPASS_EXTERNAL,
            0u,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

        subpass.AddDependencies(
            0u,
            VK_SUBPASS_EXTERNAL,
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT);

        VkRect2D renderArea{
            { 0u, 0u },
            m_Extent
        };

        Array<VkClearValue> clearValues;
        clearValues.Resize(1);
        clearValues[0].depthStencil = {
            1.0f, // float    depth
            0u    // uint32_t stencil
        };

        m_RenderPass.Create(attachment, subpass, renderArea, Move(clearValues));
        m_RenderPass.UpdateRenderArea({ {}, m_Extent });

        Shader shader("shadowmap.vert", "shadowmap.frag");

        VertexLayout vertexLayout;
        vertexLayout.AddBinding(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
        vertexLayout.AddAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, ADH_OFFSET(Vertex, position));
        vertexLayout.AddAttribute(1, 0, VK_FORMAT_R32G32B32_SFLOAT, ADH_OFFSET(Vertex, normals));
        vertexLayout.AddAttribute(2, 0, VK_FORMAT_R32G32_SFLOAT, ADH_OFFSET(Vertex, textureCoords));
        vertexLayout.Create();

        pipelineLayout.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
        pipelineLayout.CreateSet();

        pipelineLayout.AddPushConstant(VK_SHADER_STAGE_VERTEX_BIT, sizeof(xmm::Matrix), 0);
        pipelineLayout.Create();

        graphicsPipeline.Create(shader, vertexLayout, pipelineLayout, m_RenderPass,
                                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 0.0f, VK_TRUE);

        descriptorSet.Initialize(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 3);
        descriptorSet.AddPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
        descriptorSet.Create(pipelineLayout.GetSetLayout());

        lightSpaceBuffer.Create(&lightSpace, sizeof(lightSpace), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        descriptorSet.Update(
            lightSpaceBuffer.GetDescriptor(),
            0u,                               // descriptor index
            0u,                               // binding
            0u,                               // array element
            1u,                               // array count
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER // type
        );

        lightSpaceBuffer.Update();

        m_Image.Create(
            { m_Extent.width, m_Extent.height, 1u },
            tools::GetSupportedDepthFormat(Context::Get()->GetPhysicalDevice()),
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_TYPE_2D,
            (VkImageCreateFlagBits)0,
            1,
            1,
            VK_SAMPLE_COUNT_1_BIT,
            VkImageUsageFlagBits(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT),
            VK_IMAGE_ASPECT_DEPTH_BIT,
            VK_IMAGE_VIEW_TYPE_2D,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            VK_SHARING_MODE_EXCLUSIVE);

        VkImageView viewAttachments[]{
            m_Image.GetImageView()
        };

        descriptor.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        descriptor.sampler     = sampler;
        descriptor.imageView   = m_Image.GetImageView();

        m_Framebuffer.Create(m_RenderPass, std::size(viewAttachments), viewAttachments, m_Extent, 1u);

        debug.Create(renderPass, descriptor);
    }

    void Recreate(VkExtent2D extent) {
        m_Extent = extent;
        auto device{ Context::Get()->GetDevice() };
        vkDeviceWaitIdle(device);

        m_Framebuffer.Destroy();

        m_Image.Destroy();

        m_Image.Create(
            { m_Extent.width, m_Extent.height, 1u },
            tools::GetSupportedDepthFormat(Context::Get()->GetPhysicalDevice()),
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_TYPE_2D,
            (VkImageCreateFlagBits)0,
            1,
            1,
            VK_SAMPLE_COUNT_1_BIT,
            VkImageUsageFlagBits(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT),
            VK_IMAGE_ASPECT_DEPTH_BIT,
            VK_IMAGE_VIEW_TYPE_2D,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            VK_SHARING_MODE_EXCLUSIVE);

        m_RenderPass.UpdateRenderArea({ {}, m_Extent });

        VkImageView viewAttachments[]{
            m_Image.GetImageView()
        };

        m_Framebuffer.Create(m_RenderPass, std::size(viewAttachments), viewAttachments, m_Extent, 1u);
    }

    RenderPass m_RenderPass;
    VkExtent2D m_Extent;
    vk::Image m_Image;
    Framebuffer m_Framebuffer;

    xmm::Matrix lightSpace{ 1.0f };
    UniformBuffer lightSpaceBuffer;

    PipelineLayout pipelineLayout;
    DescriptorSet descriptorSet;
    GraphicsPipeline graphicsPipeline;

    Viewport viewport;
    Scissor scissor;

    VkDescriptorImageInfo descriptor;

    Debug debug;
};

struct HDRBuffer {
    void Create(const adh::Window& window, Swapchain& swapchain, const Sampler& sampler) {
        Attachment attachment;
        attachment.AddDescription(
            VK_FORMAT_R32G32B32A32_SFLOAT,
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            Attachment::Type::eColor);

        attachment.AddDescription(
            tools::GetSupportedDepthFormat(Context::Get()->GetPhysicalDevice()),
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            Attachment::Type::eDepth);

        Subpass subpass;
        subpass.AddDescription(VK_PIPELINE_BIND_POINT_GRAPHICS, attachment);
        subpass.AddDependencies(
            VK_SUBPASS_EXTERNAL,
            0u,
            VkPipelineStageFlagBits(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT),
            VkPipelineStageFlagBits(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT),
            VK_ACCESS_NONE_KHR,
            VkAccessFlagBits(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT));

        subpass.AddDependencies(
            0u,
            VK_SUBPASS_EXTERNAL,
            VkPipelineStageFlagBits(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT),
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            VkAccessFlagBits(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT),
            VK_ACCESS_NONE_KHR);

        VkRect2D renderArea{
            { 0u, 0u },
            { (uint32_t)window.GetWindowWidth(), (uint32_t)window.GetWindowHeight() }
        };

        Array<VkClearValue> clearValues;
        clearValues.Resize(2);
        clearValues[0].color        = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = {
            1.0f, // float    depth
            0u    // uint32_t stencil
        };

        m_RenderPass.Create(attachment, subpass, renderArea, Move(clearValues));
        m_RenderPass.UpdateRenderArea({ {}, swapchain.GetExtent() });

        Shader shader("pbr.vert", "pbr.frag");

        VertexLayout vertexLayout;
        vertexLayout.AddBinding(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
        vertexLayout.AddAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, ADH_OFFSET(Vertex, position));
        vertexLayout.AddAttribute(1, 0, VK_FORMAT_R32G32B32_SFLOAT, ADH_OFFSET(Vertex, normals));
        vertexLayout.AddAttribute(2, 0, VK_FORMAT_R32G32_SFLOAT, ADH_OFFSET(Vertex, textureCoords));
        vertexLayout.Create();

        pipelineLayout.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
        pipelineLayout.AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
        pipelineLayout.CreateSet();

        pipelineLayout.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        pipelineLayout.AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        pipelineLayout.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        pipelineLayout.AddBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        pipelineLayout.CreateSet();

        pipelineLayout.AddPushConstant(VK_SHADER_STAGE_VERTEX_BIT, sizeof(xmm::Matrix), 0);
        pipelineLayout.AddPushConstant(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(Material), 64);

        pipelineLayout.Create();

        graphicsPipeline.Create(shader, vertexLayout, pipelineLayout, m_RenderPass,
                                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 0.0f, VK_TRUE);

        m_Image.Create(
            { swapchain.GetExtent().width, swapchain.GetExtent().height, 1u },
            VK_FORMAT_R32G32B32A32_SFLOAT,
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
            m_Image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_IMAGE_ASPECT_COLOR_BIT);

        VkImageView viewAttachments[]{
            m_Image.GetImageView(),
            swapchain.GetDepthBuffer().GetImageView()
        };

        m_Framebuffer.Create(m_RenderPass, std::size(viewAttachments), viewAttachments, swapchain.GetExtent(), 1u);

        descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptor.sampler     = sampler;
        descriptor.imageView   = m_Image.GetImageView();
    }

    void Recreate(Swapchain& swapchain) {
        m_Image.Destroy();

        m_Framebuffer.Destroy();

        m_Image.Create(
            { swapchain.GetExtent().width, swapchain.GetExtent().height, 1u },
            VK_FORMAT_R32G32B32A32_SFLOAT,
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
            m_Image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_IMAGE_ASPECT_COLOR_BIT);

        VkImageView viewAttachments[]{
            m_Image.GetImageView(),
            swapchain.GetDepthBuffer().GetImageView()
        };

        m_Framebuffer.Create(m_RenderPass, std::size(viewAttachments), viewAttachments, swapchain.GetExtent(), 1u);

        descriptor.imageView = m_Image.GetImageView();
        m_RenderPass.UpdateRenderArea({ {}, swapchain.GetExtent() });
    }

    vk::Image m_Image;
    Framebuffer m_Framebuffer;
    RenderPass m_RenderPass;

    PipelineLayout pipelineLayout;
    GraphicsPipeline graphicsPipeline;

    VkDescriptorImageInfo descriptor;
};

struct HDRDraw {
    void Create(RenderPass& renderPass, VkDescriptorImageInfo& info, VkDescriptorImageInfo& info2) {
        Shader shader("hdr.vert", "hdr.frag");

        VertexLayout vertexLayout;
        vertexLayout.Create();

        pipelineLayout.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        pipelineLayout.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        pipelineLayout.CreateSet();

        pipelineLayout.AddPushConstant(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(float) * 2, 0);

        pipelineLayout.Create();

        graphicsPipeline.Create(shader, vertexLayout, pipelineLayout, renderPass,
                                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 0.0f, VK_TRUE);

        descriptorSet.Initialize(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 3);
        descriptorSet.AddPool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);
        descriptorSet.Create(pipelineLayout.GetSetLayout());

        descriptorSet.Update(
            info,
            0u,                                       // descriptor index
            1u,                                       // binding
            0u,                                       // array element
            1u,                                       // array count
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
        );
        descriptorSet.Update(
            info2,
            0u,                                       // descriptor index
            2u,                                       // binding
            0u,                                       // array element
            1u,                                       // array count
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
        );
    }

    void Update(VkDescriptorImageInfo& info, VkDescriptorImageInfo& info2) {
        descriptorSet.Update(
            info,
            0u,                                       // descriptor index
            1u,                                       // binding
            0u,                                       // array element
            1u,                                       // array count
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
        );
        descriptorSet.Update(
            info2,
            0u,                                       // descriptor index
            2u,                                       // binding
            0u,                                       // array element
            1u,                                       // array count
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
        );
    }

    DescriptorSet descriptorSet;
    PipelineLayout pipelineLayout;
    GraphicsPipeline graphicsPipeline;

    float intensity[2]{
        1.0f,
        1.0f
    };
};

struct GaussianBlur {
    void Create(const adh::Window& window, Swapchain& swapchain, const Sampler& sampler, VkDescriptorImageInfo imageInfo) {
        auto e = swapchain.GetExtent();
        e.width /= 2;
        e.height /= 2;

        brightColor.extent = e;

        e.width /= 2;
        e.height /= 2;
        extents.EmplaceBack(e);
        while (e.width > 20 && e.height > 20) {
            e.width /= 2;
            e.height /= 2;
            extents.EmplaceBack(e);
        }

        uboUniformBuffer.Create(&ubo, sizeof(ubo), swapchain.GetImageViewCount(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

        Attachment attachment;
        attachment.AddDescription(
            VK_FORMAT_R32G32B32A32_SFLOAT,
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            Attachment::Type::eColor);

        Subpass subpass;
        subpass.AddDescription(VK_PIPELINE_BIND_POINT_GRAPHICS, attachment);
        subpass.AddDependencies(
            VK_SUBPASS_EXTERNAL,
            0u,
            VkPipelineStageFlagBits(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT),
            VkPipelineStageFlagBits(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT),
            VK_ACCESS_NONE_KHR,
            VkAccessFlagBits(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT));

        subpass.AddDependencies(
            0u,
            VK_SUBPASS_EXTERNAL,
            VkPipelineStageFlagBits(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT),
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            VkAccessFlagBits(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT),
            VK_ACCESS_NONE_KHR);

        VkRect2D renderArea{
            { 0u, 0u },
            { (uint32_t)window.GetWindowWidth(), (uint32_t)window.GetWindowHeight() }
        };

        Array<VkClearValue> clearValues;
        clearValues.Resize(1);
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };

        mRenderPass.Create(attachment, subpass, renderArea, Move(clearValues));

        // Bright color pass
        {
            Shader shader("brightColor.vert", "brightColor.frag");
            VertexLayout vertexLayout;
            vertexLayout.Create();

            brightColor.pipelineLayout.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
            brightColor.pipelineLayout.CreateSet();

            brightColor.pipelineLayout.AddPushConstant(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(float), 0);

            brightColor.pipelineLayout.Create();

            brightColor.graphicsPipeline.Create(shader, vertexLayout, brightColor.pipelineLayout, mRenderPass,
                                                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_CULL_MODE_FRONT_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                                VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 0.0f, VK_TRUE);

            brightColor.mImages.Create(
                { brightColor.extent.width, brightColor.extent.height, 1u },
                VK_FORMAT_R32G32B32A32_SFLOAT,
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
                brightColor.mImages,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_IMAGE_ASPECT_COLOR_BIT);

            VkImageView viewAttachments[]{
                brightColor.mImages.GetImageView()
            };

            brightColor.mFramebuffers.Create(mRenderPass, std::size(viewAttachments), viewAttachments, brightColor.extent, 1u);

            brightColor.imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            brightColor.imageInfo.sampler     = sampler;
            brightColor.imageInfo.imageView   = brightColor.mImages.GetImageView();

            brightColor.descriptorSets.Initialize(VK_PIPELINE_BIND_POINT_GRAPHICS, brightColor.pipelineLayout, swapchain.GetImageViewCount());
            brightColor.descriptorSets.AddPool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
            brightColor.descriptorSets.Create(brightColor.pipelineLayout.GetSetLayout());

            brightColor.descriptorSets.Update(
                imageInfo,
                0u,                                       // descriptor index
                1u,                                       // binding
                0u,                                       // array element
                1u,                                       // array count
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
            );
        }

        // Blur passes
        {
            int blurPasses = extents.GetSize() * 2;

            Shader shader("gaussianBlur.vert", "gaussianBlur.frag");

            VertexLayout vertexLayout;
            vertexLayout.Create();

            pipelineLayout.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
            pipelineLayout.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
            pipelineLayout.CreateSet();

            pipelineLayout.AddPushConstant(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(int) * 3, 0);

            pipelineLayout.Create();

            graphicsPipeline.Create(shader, vertexLayout, pipelineLayout, mRenderPass,
                                    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_CULL_MODE_FRONT_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                    VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 0.0f, VK_TRUE);

            mImages.Resize(blurPasses);
            mFramebuffers.Resize(blurPasses);
            descriptorImageInfos.Resize(blurPasses);

            int count = 0;
            for (int i{}; i != mImages.GetSize(); ++i) {
                mImages[i].Create(
                    { extents[count].width, extents[count].height, 1u },
                    VK_FORMAT_R32G32B32A32_SFLOAT,
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
                    mImages[i],
                    VK_IMAGE_LAYOUT_UNDEFINED,
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    VK_IMAGE_ASPECT_COLOR_BIT);

                VkImageView viewAttachments[]{
                    mImages[i].GetImageView()
                };

                mFramebuffers[i].Create(mRenderPass, std::size(viewAttachments), viewAttachments, extents[count], 1u);

                descriptorImageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                descriptorImageInfos[i].sampler     = sampler;
                descriptorImageInfos[i].imageView   = mImages[i].GetImageView();

                if (i % 2) {
                    count++;
                }
            }

            descriptorSets.Resize(blurPasses);
            for (int i{}; i != blurPasses; ++i) {
                descriptorSets[i].Initialize(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, swapchain.GetImageViewCount());
                descriptorSets[i].AddPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
                descriptorSets[i].AddPool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
                descriptorSets[i].Create(pipelineLayout.GetSetLayout());
            }

            descriptorSets[0].Update(
                uboUniformBuffer.GetDescriptor(),
                0u,                               // descriptor index
                0u,                               // binding
                0u,                               // array element
                1u,                               // array count
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER // type
            );
            descriptorSets[0].Update(
                brightColor.imageInfo,
                0u,                                       // descriptor index
                1u,                                       // binding
                0u,                                       // array element
                1u,                                       // array count
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
            );

            for (int i{ 1 }; i != descriptorSets.GetSize(); ++i) {
                descriptorSets[i].Update(
                    uboUniformBuffer.GetDescriptor(),
                    0u,                               // descriptor index
                    0u,                               // binding
                    0u,                               // array element
                    1u,                               // array count
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER // type
                );
                descriptorSets[i].Update(
                    descriptorImageInfos[i - 1],
                    0u,                                       // descriptor index
                    1u,                                       // binding
                    0u,                                       // array element
                    1u,                                       // array count
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
                );
            }
        }
        // Recompose
        {
            int recomposePasses = extents.GetSize();

            Shader shader("bloomRecompose.vert", "bloomRecompose.frag");

            VertexLayout vertexLayout;
            vertexLayout.Create();

            recompose.pipelineLayout.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
            recompose.pipelineLayout.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
            recompose.pipelineLayout.CreateSet();

            recompose.pipelineLayout.AddPushConstant(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(int) * 2, 0);

            recompose.pipelineLayout.Create();

            recompose.graphicsPipeline.Create(shader, vertexLayout, recompose.pipelineLayout, mRenderPass,
                                              VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_CULL_MODE_FRONT_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                              VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 0.0f, VK_TRUE);

            int s = recomposePasses - 1;
            recompose.flickerImages.Resize(s);
            recompose.flickerFramebuffers.Resize(s);
            recompose.flickerImageInfo.Resize(s);

            for (int i{}; i != s; ++i) {
                recompose.flickerImages[i].Create(
                    { extents[extents.GetSize() - 2 - i].width, extents[extents.GetSize() - 2 - i].height, 1u },
                    VK_FORMAT_R32G32B32A32_SFLOAT,
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
                    recompose.flickerImages[i],
                    VK_IMAGE_LAYOUT_UNDEFINED,
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    VK_IMAGE_ASPECT_COLOR_BIT);

                VkImageView viewAttachments[]{
                    recompose.flickerImages[i].GetImageView()
                };

                recompose.flickerFramebuffers[i].Create(mRenderPass, std::size(viewAttachments), viewAttachments, extents[extents.GetSize() - 2 - i], 1u);

                recompose.flickerImageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                recompose.flickerImageInfo[i].sampler     = sampler;
                recompose.flickerImageInfo[i].imageView   = recompose.flickerImages[i].GetImageView();
            }

            recompose.mImages.Create(
                { brightColor.extent.width, brightColor.extent.height, 1u },
                VK_FORMAT_R32G32B32A32_SFLOAT,
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
                recompose.mImages,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_IMAGE_ASPECT_COLOR_BIT);

            VkImageView viewAttachments[]{
                recompose.mImages.GetImageView()
            };

            recompose.mFramebuffers.Create(mRenderPass, std::size(viewAttachments), viewAttachments, brightColor.extent, 1u);

            recompose.imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            recompose.imageInfo.sampler     = sampler;
            recompose.imageInfo.imageView   = recompose.mImages.GetImageView();

            recompose.descriptorSets.Resize(recomposePasses);
            for (int i{}; i != recomposePasses; ++i) {
                recompose.descriptorSets[i].Initialize(VK_PIPELINE_BIND_POINT_GRAPHICS, recompose.pipelineLayout, swapchain.GetImageViewCount());
                recompose.descriptorSets[i].AddPool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);
                recompose.descriptorSets[i].Create(recompose.pipelineLayout.GetSetLayout());
            }

            recompose.descriptorSets[0].Update(
                descriptorImageInfos[descriptorImageInfos.GetSize() - 3],
                0u,                                       // descriptor index
                1u,                                       // binding
                0u,                                       // array element
                1u,                                       // array count
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
            );
            recompose.descriptorSets[0].Update(
                descriptorImageInfos[descriptorImageInfos.GetSize() - 1],
                0u,                                       // descriptor index
                2u,                                       // binding
                0u,                                       // array element
                1u,                                       // array count
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
            );

            int count = descriptorImageInfos.GetSize() - 2;
            for (int i{ 1 }; i != recompose.descriptorSets.GetSize() - 1; ++i) {
                recompose.descriptorSets[i].Update(
                    descriptorImageInfos[count - 1 - (2 * i)],
                    0u,                                       // descriptor index
                    1u,                                       // binding
                    0u,                                       // array element
                    1u,                                       // array count
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
                );
                recompose.descriptorSets[i].Update(
                    recompose.flickerImageInfo[i - 1],
                    0u,                                       // descriptor index
                    2u,                                       // binding
                    0u,                                       // array element
                    1u,                                       // array count
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
                );
            }

            recompose.descriptorSets[recompose.descriptorSets.GetSize() - 1].Update(
                brightColor.imageInfo,
                0u,                                       // descriptor index
                1u,                                       // binding
                0u,                                       // array element
                1u,                                       // array count
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
            );
            recompose.descriptorSets[recompose.descriptorSets.GetSize() - 1].Update(
                recompose.flickerImageInfo[s - 1],
                0u,                                       // descriptor index
                2u,                                       // binding
                0u,                                       // array element
                1u,                                       // array count
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
            );
        }

        finalPassdescriptor = recompose.imageInfo;
    }

    void Recreate(Swapchain& swapchain, VkDescriptorImageInfo info) {
        brightColor.descriptorSets.Update(
            info,
            0u,                                       // descriptor index
            1u,                                       // binding
            0u,                                       // array element
            1u,                                       // array count
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
        );
    }

    void Draw(VkCommandBuffer cmd, std::uint32_t imageIndex, Viewport& viewport, Scissor& scissor) {
        // Bright color
        {
            viewport.Update(brightColor.extent, false);
            viewport.Set(cmd);

            scissor.Update(brightColor.extent);
            scissor.Set(cmd);

            mRenderPass.UpdateRenderArea({ {}, brightColor.extent });
            mRenderPass.Begin(cmd, brightColor.mFramebuffers);
            brightColor.graphicsPipeline.Bind(cmd);
            brightColor.descriptorSets.Bind(cmd, imageIndex);

            vkCmdPushConstants(
                cmd,
                brightColor.pipelineLayout,
                VK_SHADER_STAGE_FRAGMENT_BIT,
                0u,
                sizeof(float), &brightColor.threshold);

            vkCmdDraw(cmd, 3, 1, 0, 0);
            mRenderPass.End(cmd);
        }

        // Blur
        {
            uboUniformBuffer.Update(imageIndex);
            int count      = 0;
            horizontalBlur = 1;
            for (int i{}; i != mFramebuffers.GetSize(); ++i) {
                viewport.Update(extents[count], false);
                viewport.Set(cmd);
                scissor.Update(extents[count]);
                scissor.Set(cmd);

                mRenderPass.UpdateRenderArea({ {}, extents[count] });
                mRenderPass.Begin(cmd, mFramebuffers[i]);
                graphicsPipeline.Bind(cmd);
                descriptorSets[i].Bind(cmd, imageIndex);

                horizontalBlur = horizontalBlur == 1 ? 0 : 1;
                if (!horizontalBlur) {
                    if (!i) {
                        int p[3]{ horizontalBlur, (int)brightColor.extent.width, (int)brightColor.extent.height };
                        vkCmdPushConstants(
                            cmd,
                            pipelineLayout,
                            VK_SHADER_STAGE_FRAGMENT_BIT,
                            0u,
                            sizeof(int) * 3, p);
                    } else {
                        int p[3]{ horizontalBlur, (int)extents[count - 1].width, (int)extents[count - 1].height };
                        vkCmdPushConstants(
                            cmd,
                            pipelineLayout,
                            VK_SHADER_STAGE_FRAGMENT_BIT,
                            0u,
                            sizeof(int) * 3, p);
                    }
                } else {
                    int p[3]{ horizontalBlur, (int)extents[count].width, (int)extents[count].height };
                    vkCmdPushConstants(
                        cmd,
                        pipelineLayout,
                        VK_SHADER_STAGE_FRAGMENT_BIT,
                        0u,
                        sizeof(int) * 3, p);
                }

                vkCmdDraw(cmd, 3, 1, 0, 0);
                mRenderPass.End(cmd);

                if (i % 2) {
                    count++;
                }
            }
        }

        // Recompose
        {
            int size        = descriptorImageInfos.GetSize() - 4;
            int extentIndex = extents.GetSize() - 2;
            for (int i{}; i != recompose.descriptorSets.GetSize() - 1; ++i) {
                viewport.Update(extents[extentIndex], false);
                viewport.Set(cmd);
                scissor.Update(extents[extentIndex]);
                scissor.Set(cmd);

                mRenderPass.UpdateRenderArea({ {}, extents[extentIndex] });
                extentIndex--;

                mRenderPass.Begin(cmd, recompose.flickerFramebuffers[i]);
                size -= 2;

                recompose.graphicsPipeline.Bind(cmd);

                recompose.descriptorSets[i].Bind(cmd, imageIndex);

                int p[2]{ (int)extents[extentIndex + 1].width, (int)extents[extentIndex + 1].height };

                vkCmdPushConstants(
                    cmd,
                    recompose.pipelineLayout,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
                    0u,
                    sizeof(int) * 2, p);

                vkCmdDraw(cmd, 3, 1, 0, 0);
                mRenderPass.End(cmd);
            }

            viewport.Update(brightColor.extent, false);
            viewport.Set(cmd);
            scissor.Update(brightColor.extent);
            scissor.Set(cmd);

            mRenderPass.UpdateRenderArea({ {}, brightColor.extent });

            mRenderPass.Begin(cmd, recompose.mFramebuffers);

            recompose.graphicsPipeline.Bind(cmd);

            recompose.descriptorSets[recompose.descriptorSets.GetSize() - 1].Bind(cmd, imageIndex);

            vkCmdDraw(cmd, 3, 1, 0, 0);
            mRenderPass.End(cmd);
        }
    }

    Array<Image> mImages;
    Array<Framebuffer> mFramebuffers;

    Array<DescriptorSet> descriptorSets;
    Array<VkDescriptorImageInfo> descriptorImageInfos;

    VkDescriptorImageInfo finalPassdescriptor;

    PipelineLayout pipelineLayout;
    GraphicsPipeline graphicsPipeline;

    RenderPass mRenderPass;

    struct UBO {
        float blurScale{ 0.25f };
        float blurStrength{ 0.25f };
    };

    int horizontalBlur = 1;
    UBO ubo;
    UniformBuffer uboUniformBuffer;

    Array<VkExtent2D> extents;

    struct BrightColor {
        VkExtent2D extent;
        Image mImages;
        Framebuffer mFramebuffers;

        DescriptorSet descriptorSets;
        VkDescriptorImageInfo imageInfo;

        PipelineLayout pipelineLayout;
        GraphicsPipeline graphicsPipeline;

        float threshold = 1.0f;
    } brightColor;

    struct Recompose {
        PipelineLayout pipelineLayout;
        Array<DescriptorSet> descriptorSets;
        GraphicsPipeline graphicsPipeline;

        Image mImages;
        Framebuffer mFramebuffers;
        VkDescriptorImageInfo imageInfo;

        Array<Image> flickerImages;
        Array<Framebuffer> flickerFramebuffers;
        Array<VkDescriptorImageInfo> flickerImageInfo;
    } recompose;
};

struct CollisionPair {
    std::uint64_t e[2];
    CollisionEvent::Type type;
};

class AdHoc {
  public:
    const char* name{ "AdHoc" };
    adh::Window window;
    Context context;
    Swapchain swapchain;
    std::uint32_t swapchanImageCount{ 2 };

    Sampler sampler;
    Scene scene;
    Editor editor;
    Array<Framebuffer> swapchainFramebuffers;
    PipelineLayout pipelineLayout;
    DescriptorSet descriptorSet;
    DescriptorSet editorDescriptorSet;
    Viewport viewport;
    Scissor scissor;
    RenderPass renderPass;
    VkQueue graphicsQueue;
    GraphicsPipeline graphicsPipeline;
    CommandPool commandPool;
    CommandBuffer commandBuffer;
    Array<VkFence> fence1;
    Array<VkFence> fence2;
    Array<VkSemaphore> presentSempahore;
    Array<VkSemaphore> renderSemaphore;
    Input input;
    UniformBuffer viewProjectionBuffer;
    UniformBuffer editorViewProjectionBuffer;
    UniformBuffer lightSpaceBuffer;
    UniformBuffer fragDataBuffer;
    UniformBuffer lightBuffer;

    std::uint32_t currentFrame{};
    std::uint32_t imageIndex{};
    bool renderingReady = false;
    Material material;
    DirectionalLight directionalLight;

    DescriptorSet editorDescriptorSet2;
    UniformBuffer editorViewProjectionBuffer2;
    PBR_UBO editorViewProjection2;

    AspectRatio g_AspectRatio;
    bool g_DrawEditor{ true };
    bool g_EditorFpsLimit{ true };
    bool g_MaximizeOnPlay{ false };
    bool maximizeOnPlay2{ false };

    bool g_IsPlaying{ false };
    bool g_IsPaused{ false };
    bool g_AreScriptsReady{ false };

    std::vector<std::function<void()>> collisionCallbacks;
    std::vector<CollisionPair> collisionCallbacks2;

    bool clearFramebuffers     = true;
    int clearFramebuffersCount = 0;

    ShadowMap2D shadowMap;
    HDRBuffer hdrBuffer;
    HDRDraw hdrDraw;
    GaussianBlur gaussianBlur;

    float* floats[7];

    int shadowPCF        = 0;
    float floatShadowPCF = 0;

    AudioDevice audioDevice;

    Texture2D testTexture;
    Texture2D testTexture2;

  public:
    ~AdHoc() {
        auto device{ Context::Get()->GetDevice() };
        vkDeviceWaitIdle(device);
        for (int i{}; i != swapchain.GetImageViewCount(); ++i) {
            vkDestroySemaphore(device, presentSempahore[i], nullptr);
            vkDestroySemaphore(device, renderSemaphore[i], nullptr);
            vkDestroyFence(device, fence1[i], nullptr);
        }
        Mesh::Clear(); // TODO: temp
        Texture2D::CleanUpDefaultSamplers();
    }

    void OnCollisionEvent(CollisionEvent* event) {
        if (g_IsPlaying && g_AreScriptsReady) {
            CollisionPair p;
            p.e[0] = event->entityA;
            p.e[1] = event->entityB;
            p.type = event->type;

            collisionCallbacks2.emplace_back(p);
            // scene.GetWorld().GetSystem<lua::Script>().ForEach([&](ecs::Entity ent, lua::Script& script) {
            //     bool call{};
            //     EntityID rhs{};
            //     EntityID e = static_cast<EntityID>(ent);

            //     if (e == event->entityA) {
            //         rhs  = event->entityB;
            //         call = true;
            //     } else if (e == event->entityB) {
            //         rhs  = event->entityA;
            //         call = true;
            //     }

            //     if (call) {
            //         collisionCallbacks.emplace_back([&scene = scene, ent = ent, eventType = event->type, rhs = rhs]() {
            //             auto [script] = scene.GetWorld().Get<lua::Script>(ent);
            //             script.Bind();
            //             switch (eventType) {
            //             case CollisionEvent::Type::eCollisionEnter:
            //                 {
            //                     script.Call("OnCollisionEnter", rhs);
            //                     break;
            //                 }
            //             case CollisionEvent::Type::eCollisionPersist:
            //                 {
            //                     script.Call("OnCollisionPersist", rhs);
            //                     break;
            //                 }
            //             case CollisionEvent::Type::eCollisionExit:
            //                 {
            //                     script.Call("OnCollisionExit", rhs);
            //                     break;
            //                 }
            //             case CollisionEvent::Type::eTriggerEnter:
            //                 {
            //                     script.Call("OnTriggerEnter", rhs);
            //                     break;
            //                 }
            //             case CollisionEvent::Type::eTriggerPersist:
            //                 {
            //                     script.Call("OnTriggerPersist", rhs);
            //                     break;
            //                 }
            //             case CollisionEvent::Type::eTriggerExit:
            //                 {
            //                     script.Call("OnTriggerExit", rhs);
            //                     break;
            //                 }
            //             }
            //             script.Unbind();
            //         });
            //     }
            // });
        }
        event->isHandled = true;
    }

    void OnStatusEvent(StatusEvent* event) {
        switch (event->type) {
        case StatusEvent::Type::eRun:
            {
                scene.Save();
                scene.ResetPhysicsWorld();
                ReadyScript();
                g_AreScriptsReady = true;
                g_IsPlaying       = true;
                g_IsPaused        = false;

                maximizeOnPlay2 = g_MaximizeOnPlay;
                collisionCallbacks.clear();
                break;
            }
        case StatusEvent::Type::eStop:
            {
                if (g_IsPlaying) {
                    Mesh::Clear(); // TODO: temp
                    scene.Load();
                    scene.GetState().ClearStack();
                    g_AreScriptsReady = false;
                    g_IsPlaying       = false;
                    g_IsPaused        = false;
                    g_MaximizeOnPlay  = maximizeOnPlay2;
                    collisionCallbacks.clear();
                }
                break;
            }
        case StatusEvent::Type::ePause:
            {
                g_IsPaused = true;
                break;
            }
        case StatusEvent::Type::eUnpause:
            {
                g_IsPaused = false;
                break;
            }
        }
        event->isHandled = true;
    }

    void UpdateTexture(const Texture2D& t) {
        // TODO: Texture!!
        descriptorSet.Update(
            t.GetDescriptor(),
            1u,                                       // descriptor index
            3u,                                       // binding
            0u,                                       // array element
            1u,                                       // array count
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
        );
        // TODO: Texture!!
        editorDescriptorSet.Update(
            t.GetDescriptor(),
            1u,                                       // descriptor index
            3u,                                       // binding
            0u,                                       // array element
            1u,                                       // array count
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
        );
        // TODO: Texture!!
        editorDescriptorSet2.Update(
            t.GetDescriptor(),
            1u,                                       // descriptor index
            3u,                                       // binding
            0u,                                       // array element
            1u,                                       // array count
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
        );
    }

    void Initialize(const char* path) {
        window.Create(name, 1200, 800, true, false);
        context.Create(window, "AdHoc", path);
        swapchain.Create(swapchanImageCount, VK_FORMAT_B8G8R8A8_UNORM, VK_PRESENT_MODE_FIFO_KHR);
        InitializeRenderPass();

        sampler.Create(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_COMPARE_OP_NEVER, VK_FALSE, VK_TRUE);

        // Texture2D::InitializeDefaultSamplers();

        shadowMap.lightSpace = xmm::PerspectiveLH(ToRadians(140.0f), 1.0f, 1.0f, 1000.0f) * xmm::LookAtLH(sunPosition, { 0, 0, 0 }, { 0, 1, 0 });
        shadowMap.m_Extent   = { 2048, 2048 };
        // shadowMap.m_Extent = { 1024, 1024 };
        shadowMap.Create(renderPass, sampler);
        lightSpace = lightSpaceBias * shadowMap.lightSpace;

        // TODO: Textures
        testTexture.Create((Context::Get()->GetDataDirectory() + "Assets/Textures/" + "link.tga").data(), VK_IMAGE_USAGE_SAMPLED_BIT, &sampler);
        testTexture2.Create((Context::Get()->GetDataDirectory() + "Assets/Textures/" + "wall.tga").data(), VK_IMAGE_USAGE_SAMPLED_BIT, &sampler);

        InitializePipeline();
        InitializeDescriptorSets();
        InitializeEditorDescriptorSets();
        commandBuffer.Create(VK_COMMAND_BUFFER_LEVEL_PRIMARY, swapchain.GetImageViewCount(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, DeviceQueues::Family::eGraphics);
        InitializeFramebuffers();
        InitializeSyncData();

        InitializeScripting();
        CreateEditor();

        auto runtimeCamera = scene.GetWorld().CreateEntity();
        scene.GetWorld().Add<Tag>(runtimeCamera, "Runtime Camera");
        auto [c2]          = scene.GetWorld().Add<Camera3D>(runtimeCamera, Camera3D{});
        c2.isRuntimeCamera = true;
        c2.eyePosition.z   = -8;

        auto sceneCamera = scene.GetWorld().CreateEntity();
        scene.GetWorld().Add<Tag>(sceneCamera, "Main Camera");
        auto [c1]        = scene.GetWorld().Add<Camera3D>(sceneCamera, Camera3D{});
        c1.isSceneCamera = true;
        c1.eyePosition   = Vector3D{ -7.0f, 8.0f, -14.0f };

        EventListener eventListener = Event::CreateListener();
        Event::AddListener<WindowEvent>(eventListener, &AdHoc::OnResize, this);
        Event::AddListener<StatusEvent>(eventListener, &AdHoc::OnStatusEvent, this);
        Event::AddListener<CollisionEvent>(eventListener, &AdHoc::OnCollisionEvent, this);
        input.Initialize();

        hdrBuffer.Create(window, swapchain, sampler);
        gaussianBlur.Create(window, swapchain, sampler, hdrBuffer.descriptor);
        hdrDraw.Create(renderPass, hdrBuffer.descriptor, gaussianBlur.finalPassdescriptor);

        floats[0] = &hdrDraw.intensity[0];
        floats[1] = &gaussianBlur.brightColor.threshold;
        floats[2] = &gaussianBlur.ubo.blurScale;
        floats[3] = &gaussianBlur.ubo.blurStrength;
        floats[4] = &directionalLight.intensity;
        floats[5] = &hdrDraw.intensity[1];
        floats[6] = &floatShadowPCF;

        audioDevice.Create();

        UpdateTexture(testTexture2);

        renderingReady = true;
    }

    void Run() {
        auto start            = std::chrono::steady_clock::now();
        const float maxFPS    = 60.0;
        const float maxPeriod = 1.0 / maxFPS;

        // Stopwatch timer;
        // float deltaTime;

        while (window.IsOpen()) {
            if (!renderingReady) {
                continue;
            }

            while (window.IsMinimized()) {
                window.PollEvents();
            }

            auto end        = std::chrono::steady_clock::now();
            float deltaTime = std::chrono::duration<float>(end - start).count();
            // deltaTime                = timer.Lap();
            ScriptHandler::deltaTime = deltaTime;

            if (!g_EditorFpsLimit || g_IsPlaying || deltaTime >= maxPeriod) {
                editor.OnUpdate(&scene, deltaTime, g_DrawEditor);
                input.OnUpdate();
                input.PollEvents();
                window.PollEvents();

                UpdateCameras();
                UpdateScripts(deltaTime);

                // if (!collisionCallbacks.empty()) {
                // for (auto&& i : collisionCallbacks) {
                //     i();
                // }
                //     collisionCallbacks.clear();
                // }

                if (g_IsPlaying && !g_IsPaused) {
                    scene.GetPhysics().StepSimulation(deltaTime);
                    scene.GetWorld().GetSystem<Transform, RigidBody>().ForEach([&](Transform& transform, RigidBody& rigidBody) {
                        rigidBody.OnUpdate(transform);
                    });
                }

                if (g_IsPlaying && g_MaximizeOnPlay) {
                    g_DrawEditor = false;
                } else {
                    g_DrawEditor = true;
                }

                if (editor.GetKeyDown(ADH_ESCAPE)) {
                    g_MaximizeOnPlay = false;
                }

                if (swapchain.isValid) {
                    Draw();
                } else if (!swapchain.isValid && !window.IsMinimized()) {
                    RecreateSwapchain();
                    RecreateEditor();
                }

                if (ScriptHandler::loadSceneFilename) {
                    scene.LoadFromFile((Context::Get()->GetDataDirectory() + "Assets/Scenes/" + ScriptHandler::loadSceneFilename).data());
                    ScriptHandler::loadSceneFilename = nullptr;
                    scene.GetState().ClearStack();
                    scene.ResetPhysicsWorld();
                    ReadyScript();
                }

                if (!ScriptHandler::scriptComponentEvent.IsEmpty()) {
                    for (auto&& i : ScriptHandler::scriptComponentEvent) {
                        i();
                    }
                    ScriptHandler::scriptComponentEvent.Clear();
                }
                start = end;
            }
        }
    }

    void UpdateCameras() {
        scene.GetWorld().GetSystem<Camera2D>().ForEach([&](Camera2D& camera) {
            auto width    = editor.GetSelectedAspectRatioWidth();
            auto height   = editor.GetSelectedAspectRatioHeight();
            camera.left   = -(width / 2.0f);
            camera.right  = width / 2.0f;
            camera.bottom = -(height / 2.0f);
            camera.top    = height / 2.0f;

            if (camera.isRuntimeCamera) {
                viewProjection.viewProj = camera.GetXmmProjection() * camera.GetXmmView();
                viewProjectionBuffer.Update(imageIndex);
                editorViewProjection2.viewProj = camera.GetXmmProjection() * camera.GetXmmView();
                editorViewProjectionBuffer2.Update(imageIndex);
            }
            if (camera.isSceneCamera) {
                editorViewProjection.viewProj = camera.GetXmmProjection() * camera.GetXmmView();
                editorViewProjectionBuffer.Update(imageIndex);
            }
        });
        scene.GetWorld().GetSystem<Camera3D>().ForEach([&](Camera3D& camera) {
            camera.aspectRatio = editor.GetSelectedAspectRatio();
            if (camera.isRuntimeCamera) {
                viewProjection.viewProj = camera.GetXmmProjection() * camera.GetXmmView();
                viewProjectionBuffer.Update(imageIndex);

                editorViewProjection2.viewProj = camera.GetXmmProjection() * camera.GetXmmView();
                editorViewProjectionBuffer2.Update(imageIndex);
            }
            if (camera.isSceneCamera) {
                // TODO: camera control
                if (input.GetMiddleMouseButtonDown()) {
                }

                editorViewProjection.viewProj = camera.GetXmmProjection() * camera.GetXmmView();
                editorViewProjectionBuffer.Update(imageIndex);
            }
        });
    }

    void ReadyScript() {
        scene.GetWorld().GetSystem<lua::Script>().ForEach([&](ecs::Entity ent, lua::Script& script) {
            script                       = scene.GetState().CreateScript(script.filePath);
            ScriptHandler::currentEntity = static_cast<std::uint64_t>(ent);
            script.Run();
        });

        scene.GetWorld().GetSystem<lua::Script>().ForEach([&](ecs::Entity ent, lua::Script& script) {
            script.Bind();
            ScriptHandler::currentEntity = static_cast<std::uint64_t>(ent);
            script.Call("Start");
            script.Unbind();
        });
    }

    void UpdateScripts(float deltaTime) {
        if (g_IsPlaying && !g_IsPaused && g_AreScriptsReady) {
            scene.GetWorld().GetSystem<lua::Script>().ForEach([&](ecs::Entity ent, lua::Script& script) {
                script.Bind();
                ScriptHandler::currentEntity = static_cast<std::uint64_t>(ent);
                script.Call("Update");
                script.fixedUpdateAcculumator += deltaTime;
                if (script.fixedUpdateAcculumator >= 0.017f) {
                    script.Call("FixedUpdate");
                    script.fixedUpdateAcculumator = {};
                }

                auto ee = static_cast<std::uint64_t>(ent);
                for (std::size_t i{}; i != collisionCallbacks2.size(); ++i) {
                    bool call = false;
                    std::uint64_t rhs;
                    if (collisionCallbacks2[i].e[0] == ee) {
                        rhs  = collisionCallbacks2[i].e[1];
                        call = true;
                    } else if (collisionCallbacks2[i].e[1] == ee) {
                        rhs  = collisionCallbacks2[i].e[0];
                        call = true;
                    }

                    if (call) {
                        switch (collisionCallbacks2[i].type) {
                        case CollisionEvent::Type::eCollisionEnter:
                            {
                                script.Call("OnCollisionEnter", rhs);
                                break;
                            }
                        case CollisionEvent::Type::eCollisionPersist:
                            {
                                script.Call("OnCollisionPersist", rhs);
                                break;
                            }
                        case CollisionEvent::Type::eCollisionExit:
                            {
                                script.Call("OnCollisionExit", rhs);
                                break;
                            }
                        case CollisionEvent::Type::eTriggerEnter:
                            {
                                script.Call("OnTriggerEnter", rhs);
                                break;
                            }
                        case CollisionEvent::Type::eTriggerPersist:
                            {
                                script.Call("OnTriggerPersist", rhs);
                                break;
                            }
                        case CollisionEvent::Type::eTriggerExit:
                            {
                                script.Call("OnTriggerExit", rhs);
                                break;
                            }
                        }
                    }
                }

                script.Unbind();
            });
            collisionCallbacks2.clear();
        }
    }

    void Draw() {
        auto device               = Context::Get()->GetDevice();
        constexpr auto maxTimeout = std::numeric_limits<std::uint64_t>::max();

        ADH_THROW(vkWaitForFences(device, 1u, &fence1[currentFrame], VK_TRUE, maxTimeout) == VK_SUCCESS,
                  "Failed to wait for fences!");

        auto acquireNextImage{ vkAcquireNextImageKHR(device, swapchain, maxTimeout, presentSempahore[currentFrame], nullptr, &imageIndex) };
        if (acquireNextImage == VK_ERROR_OUT_OF_DATE_KHR) {
            swapchain.isValid = false;
        }

        auto cmd = commandBuffer.Begin(currentFrame);

        fragmentUbo.shadowPCF = (int)floatShadowPCF;
        fragDataBuffer.Update(imageIndex);

        // TODO: problem with fullscreen
        if (g_DrawEditor) {
            directionalLight.direction = sunPosition;
            lightBuffer.Update(imageIndex);

            shadowMap.lightSpace = xmm::PerspectiveLH(ToRadians(140.0f), 1.0f, 1.0f, 1000.0f) * xmm::LookAtLH(sunPosition, { 0, 0, 0 }, { 0, 1, 0 });
            shadowMap.lightSpaceBuffer.Update(imageIndex);

            lightSpace = lightSpaceBias * shadowMap.lightSpace;
            lightSpaceBuffer.Update(imageIndex);
        }

        // Draw shadowmap
        {
            shadowMap.m_RenderPass.Begin(cmd, shadowMap.m_Framebuffer);
            shadowMap.graphicsPipeline.Bind(cmd);
            shadowMap.descriptorSet.Bind(cmd, imageIndex);
            shadowMap.viewport.Update(shadowMap.m_Extent, false);
            shadowMap.viewport.Set(cmd);
            shadowMap.scissor.Update(shadowMap.m_Extent);
            shadowMap.scissor.Set(cmd);

            float depthBiasConstant = 1.25f;
            float depthBiasSlope    = 1.75f;
            vkCmdSetDepthBias(cmd, depthBiasConstant, 0.0f, depthBiasSlope);

            scene.GetWorld().GetSystem<Transform, Mesh, Material>().ForEach([&](ecs::Entity e, Transform& transform, Mesh& mesh, Material& material) {
                if (mesh.toDraw) {
                    // TODO: temp
                    xmm::Matrix transformMatrix;
                    if (!g_IsPlaying) {
                        transformMatrix = transform.GetXmm();
                    } else if (g_IsPlaying && scene.GetWorld().Contains<RigidBody>(e)) {
                        transformMatrix = transform.GetXmmPhysics();
                    } else {
                        transformMatrix = transform.GetXmm();
                    }
                    vkCmdPushConstants(
                        cmd,
                        shadowMap.pipelineLayout,
                        VK_SHADER_STAGE_VERTEX_BIT,
                        0u,
                        sizeof(transformMatrix), &transformMatrix);

                    if (mesh.GetIndexCount() > 0) {
                        mesh.Bind(cmd);
                        vkCmdDrawIndexed(cmd, mesh.GetIndexCount(), 1u, 0u, 0, 0u);
                    }
                }
            });

            shadowMap.m_RenderPass.End(cmd);
        }

        // End shadowmap
        if (!g_DrawEditor) {
            // Draw hdr texture
            {
                hdrBuffer.m_RenderPass.Begin(cmd, hdrBuffer.m_Framebuffer);
                hdrBuffer.graphicsPipeline.Bind(cmd);
                descriptorSet.Bind(cmd, imageIndex);

                viewport.Update(swapchain.GetExtent(), false);
                viewport.Set(cmd);

                scissor.Update(swapchain.GetExtent());
                scissor.Set(cmd);

                float depthBiasConstant = 1.25f;
                float depthBiasSlope    = 1.75f;
                vkCmdSetDepthBias(cmd, 0, 0.0f, 0);
                // vkCmdSetDepthBias(cmd, depthBiasConstant, 0.0f, depthBiasSlope);

                scene.GetWorld().GetSystem<Transform, Mesh, Material>().ForEach([&](ecs::Entity e, Transform& transform, Mesh& mesh, Material& material) {
                    if (mesh.toDraw) {
                        xmm::Matrix transformMatrix;
                        if (!g_IsPlaying) {
                            transformMatrix = transform.GetXmm();
                        } else if (g_IsPlaying && scene.GetWorld().Contains<RigidBody>(e)) {
                            transformMatrix = transform.GetXmmPhysics();
                        } else {
                            transformMatrix = transform.GetXmm();
                        }
                        vkCmdPushConstants(
                            cmd,
                            pipelineLayout,
                            VK_SHADER_STAGE_VERTEX_BIT,
                            0u,
                            sizeof(transformMatrix), &transformMatrix);

                        vkCmdPushConstants(
                            cmd,
                            pipelineLayout,
                            VK_SHADER_STAGE_FRAGMENT_BIT,
                            64u,
                            sizeof(material), &material);

                        if (mesh.GetIndexCount() > 0) {
                            mesh.Bind(cmd);
                            vkCmdDrawIndexed(cmd, mesh.GetIndexCount(), 1u, 0u, 0, 0u);
                        }
                    }
                });

                hdrBuffer.m_RenderPass.End(cmd);
            }
            // End  hdr texture

            // Gaussian blur
            {
                vkCmdSetDepthBias(cmd, 0, 0, 0);

                gaussianBlur.Draw(cmd, imageIndex, viewport, scissor);
            }
            // Gaussian blur
        }
        if (g_DrawEditor) {
            for (std::uint32_t i{}; i != 2u; ++i) {
                editor.BeginRenderPass(cmd, imageIndex, i);
                editor.BindGraphicsPipeline(cmd);

                viewport.Update(swapchain.GetExtent(), false);
                viewport.Set(cmd);
                scissor.Update(swapchain.GetExtent());
                scissor.Set(cmd);

                if (!i) {
                    editorDescriptorSet.Bind(cmd, imageIndex);
                } else {
                    editorDescriptorSet2.Bind(cmd, imageIndex);
                }

                float depthBiasConstant = 1.25f;
                float depthBiasSlope    = 1.75f;
                vkCmdSetDepthBias(cmd, 0, 0.0f, 0);
                // vkCmdSetDepthBias(cmd, depthBiasConstant, 0.0f, depthBiasSlope);

                scene.GetWorld().GetSystem<Transform, Mesh, Material>().ForEach([&](ecs::Entity e, Transform& transform, Mesh& mesh, Material& material) {
                    if (mesh.toDraw) {
                        // TODO: temp
                        xmm::Matrix transformMatrix;
                        if (!g_IsPlaying) {
                            transformMatrix = transform.GetXmm();
                        } else if (g_IsPlaying && scene.GetWorld().Contains<RigidBody>(e)) {
                            transformMatrix = transform.GetXmmPhysics();
                        } else {
                            transformMatrix = transform.GetXmm();
                        }
                        vkCmdPushConstants(
                            cmd,
                            pipelineLayout,
                            VK_SHADER_STAGE_VERTEX_BIT,
                            0u,
                            sizeof(transformMatrix), &transformMatrix);

                        vkCmdPushConstants(
                            cmd,
                            pipelineLayout,
                            VK_SHADER_STAGE_FRAGMENT_BIT,
                            64u,
                            sizeof(material), &material);

                        if (mesh.GetIndexCount() > 0) {
                            mesh.Bind(cmd);
                            vkCmdDrawIndexed(cmd, mesh.GetIndexCount(), 1u, 0u, 0, 0u);
                        }
                    }
                });

                editor.EndRenderPass(cmd);
            }
        }

        g_AspectRatio.CalculateViewport(swapchain.GetExtent(), editor.GetSelectedAspectRatioWidth(), editor.GetSelectedAspectRatioHeight());

        if (g_DrawEditor == true) {
            clearFramebuffers = true;
            renderPass.UpdateRenderArea({ {}, swapchain.GetExtent() });
        } else if (!g_DrawEditor && !clearFramebuffers) {
            renderPass.SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            renderPass.UpdateRenderArea(g_AspectRatio.GetRect());
        } else if (!g_DrawEditor && clearFramebuffers) {
            renderPass.SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            renderPass.UpdateRenderArea({ {}, swapchain.GetExtent() });
            if (clearFramebuffersCount++ == swapchain.GetImageViewCount()) {
                clearFramebuffers      = false;
                clearFramebuffersCount = 0;
            }
        }

        renderPass.Begin(cmd, swapchainFramebuffers[imageIndex]);

        if (!g_DrawEditor) {
            // TODO: editor viewport to see shadowmap
            // Draw shadowmap
            // shadowMap.debug.graphicsPipeline.Bind(cmd);
            // shadowMap.debug.descriptorSet.Bind(cmd, imageIndex);

            // viewport.Update(g_AspectRatio.GetViewport());
            // viewport.Set(cmd);

            // scissor.Update(g_AspectRatio.GetRect());
            // scissor.Set(cmd);

            // float depthBiasConstant = 1.25f;
            // float depthBiasSlope    = 1.75f;
            // vkCmdSetDepthBias(cmd, depthBiasConstant, 0.0f, depthBiasSlope);

            // vkCmdDraw(cmd, 3, 1, 0, 0);

            // viewport.Update(g_AspectRatio.GetViewport());
            // viewport.Set(cmd);

            // scissor.Update(g_AspectRatio.GetRect());
            // scissor.Set(cmd);

            // vkCmdSetDepthBias(cmd, 0.0f, 0.0f, 0.0f);

            // graphicsPipeline.Bind(cmd);
            // descriptorSet.Bind(cmd, imageIndex);

            // scene.GetWorld().GetSystem<Transform, Mesh, Material>().ForEach([&](ecs::Entity e, Transform& transform, Mesh& mesh, Material& material) {
            //     if (mesh.toDraw) {
            //         // TODO: temp
            //         xmm::Matrix transformMatrix;
            //         if (!g_IsPlaying) {
            //             transformMatrix = transform.GetXmm();
            //         } else if (g_IsPlaying && scene.GetWorld().Contains<RigidBody>(e)) {
            //             transformMatrix = transform.GetXmmPhysics();
            //         } else {
            //             transformMatrix = transform.GetXmm();
            //         }
            //         vkCmdPushConstants(
            //             cmd,
            //             pipelineLayout,
            //             VK_SHADER_STAGE_VERTEX_BIT,
            //             0u,
            //             sizeof(transformMatrix), &transformMatrix);

            //         vkCmdPushConstants(
            //             cmd,
            //             pipelineLayout,
            //             VK_SHADER_STAGE_FRAGMENT_BIT,
            //             64u,
            //             sizeof(material), &material);

            //         if (mesh.GetIndexCount() > 0) {
            //             mesh.Bind(cmd);
            //             vkCmdDrawIndexed(cmd, mesh.GetIndexCount(), 1u, 0u, 0, 0u);
            //         }
            //     }
            // });

            viewport.Update(g_AspectRatio.GetViewport());
            viewport.Set(cmd);

            scissor.Update(g_AspectRatio.GetRect());
            scissor.Set(cmd);

            float depthBiasConstant = 0;
            float depthBiasSlope    = 0;
            vkCmdSetDepthBias(cmd, 0, 0.0f, 0);

            hdrDraw.graphicsPipeline.Bind(cmd);
            hdrDraw.descriptorSet.Bind(cmd, imageIndex);

            vkCmdPushConstants(
                cmd,
                hdrDraw.pipelineLayout,
                VK_SHADER_STAGE_FRAGMENT_BIT,
                0u,
                sizeof(float) * 2, hdrDraw.intensity);

            vkCmdDraw(cmd, 3, 1, 0, 0);

        } else {
            editor.Draw(cmd, currentFrame, &g_MaximizeOnPlay, &g_IsPlaying, &g_IsPaused, &g_EditorFpsLimit, floats, sunPosition);
        }

        renderPass.End(cmd);

        commandBuffer.End(currentFrame);

        if (fence2[imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(device, 1, &fence2[imageIndex], VK_TRUE, maxTimeout);
        }
        fence2[imageIndex] = fence1[currentFrame];

        {
            VkPipelineStageFlags waitStages[]{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            VkSemaphore waitSemaphores[]{ presentSempahore[currentFrame] };
            VkSemaphore signalSemaphores[]{ renderSemaphore[currentFrame] };
            VkCommandBuffer commandBuffers[]{ cmd };
            VkSubmitInfo submitInfo{};
            submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount   = std::size(commandBuffers);
            submitInfo.pCommandBuffers      = commandBuffers;
            submitInfo.waitSemaphoreCount   = std::size(waitSemaphores);
            submitInfo.pWaitSemaphores      = waitSemaphores;
            submitInfo.pWaitDstStageMask    = waitStages;
            submitInfo.signalSemaphoreCount = std::size(signalSemaphores);
            submitInfo.pSignalSemaphores    = signalSemaphores;

            vkResetFences(device, 1, &fence1[currentFrame]);
            ADH_THROW(vkQueueSubmit(Context::Get()->GetQueue(DeviceQueues::Family::eGraphics).queue, 1u, &submitInfo, fence1[currentFrame]) == VK_SUCCESS,
                      "Failed to submit to queue!");
        }

        {
            VkSwapchainKHR swapchains[]{ swapchain };
            VkSemaphore waitSemaphores[]{ renderSemaphore[currentFrame] };
            VkPresentInfoKHR presentInfo{};
            presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = std::size(waitSemaphores);
            presentInfo.pWaitSemaphores    = waitSemaphores;
            presentInfo.swapchainCount     = std::size(swapchains);
            presentInfo.pSwapchains        = swapchains;
            presentInfo.pImageIndices      = &imageIndex;

            auto presentQueue{ vkQueuePresentKHR(Context::Get()->GetQueue(DeviceQueues::Family::ePrensent).queue, &presentInfo) };
            if (presentQueue == VK_ERROR_OUT_OF_DATE_KHR || presentQueue == VK_SUBOPTIMAL_KHR) {
                swapchain.isValid = false;
            }
        }

        currentFrame = ++currentFrame % swapchain.GetImageViewCount();
        if (currentFrame % swapchain.GetImageViewCount()) {
            Allocator::Flush();
        }
    }

    void
    InitializeRenderPass() {
        Attachment attachment;
        attachment.AddDescription(
            VK_FORMAT_B8G8R8A8_UNORM,
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            Attachment::Type::eColor);

        attachment.AddDescription(
            tools::GetSupportedDepthFormat(Context::Get()->GetPhysicalDevice()),
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            Attachment::Type::eDepth);

        Subpass subpass;
        subpass.AddDescription(VK_PIPELINE_BIND_POINT_GRAPHICS, attachment);
        subpass.AddDependencies(
            VK_SUBPASS_EXTERNAL,
            0u,
            VkPipelineStageFlagBits(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT),
            VkPipelineStageFlagBits(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT),
            VK_ACCESS_NONE_KHR,
            VkAccessFlagBits(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT));

        subpass.AddDependencies(
            0u,
            VK_SUBPASS_EXTERNAL,
            VkPipelineStageFlagBits(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT),
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            VkAccessFlagBits(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT),
            VK_ACCESS_NONE_KHR);

        VkRect2D renderArea{
            { 0u, 0u },
            { (uint32_t)window.GetWindowWidth(), (uint32_t)window.GetWindowHeight() }
        };

        Array<VkClearValue> clearValues;
        clearValues.Resize(2);
        clearValues[0].color        = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = {
            1.0f, // float    depth
            0u    // uint32_t stencil
        };

        renderPass.Create(attachment, subpass, renderArea, Move(clearValues));
        renderPass.UpdateRenderArea({ {}, swapchain.GetExtent() });
    }

    void InitializePipeline() {
        Shader shader("pbr.vert", "pbr.frag");

        VertexLayout vertexLayout;
        vertexLayout.AddBinding(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
        vertexLayout.AddAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, ADH_OFFSET(Vertex, position));
        vertexLayout.AddAttribute(1, 0, VK_FORMAT_R32G32B32_SFLOAT, ADH_OFFSET(Vertex, normals));
        vertexLayout.AddAttribute(2, 0, VK_FORMAT_R32G32_SFLOAT, ADH_OFFSET(Vertex, textureCoords));
        vertexLayout.Create();

        pipelineLayout.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
        pipelineLayout.AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
        pipelineLayout.CreateSet();

        pipelineLayout.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        pipelineLayout.AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        pipelineLayout.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        pipelineLayout.AddBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        pipelineLayout.CreateSet();

        pipelineLayout.AddPushConstant(VK_SHADER_STAGE_VERTEX_BIT, sizeof(xmm::Matrix), 0);
        pipelineLayout.AddPushConstant(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(Material), 64);

        pipelineLayout.Create();

        graphicsPipeline.Create(shader, vertexLayout, pipelineLayout, renderPass,
                                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_CULL_MODE_FRONT_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 0.0f, VK_TRUE);
    }

    void InitializeDescriptorSets() {
        fragmentUbo.ambient        = { 0.1f, 0.1f, 0.1f };
        fragmentUbo.cameraPosition = { 0.0f, 0.0, -8.0f };

        directionalLight.direction = sunPosition;
        directionalLight.color     = { 1.0f, 1.0f, 1.0f };
        directionalLight.intensity = 10.0f;

        descriptorSet.Initialize(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, swapchain.GetImageViewCount());
        descriptorSet.AddPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4);
        descriptorSet.AddPool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);
        descriptorSet.Create(pipelineLayout.GetSetLayout());

        viewProjectionBuffer.Create(&viewProjection, sizeof(viewProjection), swapchain.GetImageViewCount(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        descriptorSet.Update(
            viewProjectionBuffer.GetDescriptor(),
            0u,                               // descriptor index
            0u,                               // binding
            0u,                               // array element
            1u,                               // array count
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER // type
        );

        lightSpaceBuffer.Create(&lightSpace, sizeof(lightSpace), swapchain.GetImageViewCount(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        descriptorSet.Update(
            lightSpaceBuffer.GetDescriptor(),
            0u,                               // descriptor index
            1u,                               // binding
            0u,                               // array element
            1u,                               // array count
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER // type
        );

        fragDataBuffer.Create(&fragmentUbo, sizeof(fragmentUbo), swapchain.GetImageViewCount(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        descriptorSet.Update(
            fragDataBuffer.GetDescriptor(),
            1u,                               // descriptor index
            0u,                               // binding
            0u,                               // array element
            1u,                               // array count
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER // type
        );

        lightBuffer.Create(&directionalLight, sizeof(directionalLight), swapchain.GetImageViewCount(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        descriptorSet.Update(
            lightBuffer.GetDescriptor(),
            1u,                               // descriptor index
            1u,                               // binding
            0u,                               // array element
            1u,                               // array count
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER // type
        );

        descriptorSet.Update(
            shadowMap.descriptor,
            1u,                                       // descriptor index
            2u,                                       // binding
            0u,                                       // array element
            1u,                                       // array count
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
        );

        // TODO: Texture!!
        descriptorSet.Update(
            testTexture.GetDescriptor(),
            1u,                                       // descriptor index
            3u,                                       // binding
            0u,                                       // array element
            1u,                                       // array count
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
        );
    }

    void InitializeEditorDescriptorSets() {
        fragmentUbo.ambient        = { 0.1f, 0.1f, 0.1f };
        fragmentUbo.cameraPosition = { 0.0f, 0.0, -8.0f };
        directionalLight.direction = sunPosition;
        directionalLight.color     = { 1.0f, 1.0f, 1.0f };
        directionalLight.intensity = 10.0f;
        {
            editorDescriptorSet.Initialize(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, swapchain.GetImageViewCount());
            editorDescriptorSet.AddPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4);
            editorDescriptorSet.AddPool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);
            editorDescriptorSet.Create(pipelineLayout.GetSetLayout());

            editorViewProjectionBuffer.Create(&editorViewProjection, sizeof(editorViewProjection),
                                              swapchain.GetImageViewCount(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
            editorDescriptorSet.Update(
                editorViewProjectionBuffer.GetDescriptor(),
                0u,                               // descriptor index
                0u,                               // binding
                0u,                               // array element
                1u,                               // array count
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER // type
            );

            editorDescriptorSet.Update(
                lightSpaceBuffer.GetDescriptor(),
                0u,                               // descriptor index
                1u,                               // binding
                0u,                               // array element
                1u,                               // array count
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER // type
            );

            editorDescriptorSet.Update(
                fragDataBuffer.GetDescriptor(),
                1u,                               // descriptor index
                0u,                               // binding
                0u,                               // array element
                1u,                               // array count
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER // type
            );

            editorDescriptorSet.Update(
                lightBuffer.GetDescriptor(),
                1u,                               // descriptor index
                1u,                               // binding
                0u,                               // array element
                1u,                               // array count
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER // type
            );

            editorDescriptorSet.Update(
                shadowMap.descriptor,
                1u,                                       // descriptor index
                2u,                                       // binding
                0u,                                       // array element
                1u,                                       // array count
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
            );
            // TODO: Texture!!
            editorDescriptorSet.Update(
                testTexture.GetDescriptor(),
                1u,                                       // descriptor index
                3u,                                       // binding
                0u,                                       // array element
                1u,                                       // array count
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
            );
        }
        {
            editorDescriptorSet2.Initialize(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, swapchain.GetImageViewCount());
            editorDescriptorSet2.AddPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4);
            editorDescriptorSet2.AddPool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2);
            editorDescriptorSet2.Create(pipelineLayout.GetSetLayout());

            editorViewProjectionBuffer2.Create(&editorViewProjection2, sizeof(editorViewProjection2),
                                               swapchain.GetImageViewCount(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
            editorDescriptorSet2.Update(
                editorViewProjectionBuffer2.GetDescriptor(),
                0u,                               // descriptor index
                0u,                               // binding
                0u,                               // array element
                1u,                               // array count
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER // type
            );

            editorDescriptorSet2.Update(
                lightSpaceBuffer.GetDescriptor(),
                0u,                               // descriptor index
                1u,                               // binding
                0u,                               // array element
                1u,                               // array count
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER // type
            );

            editorDescriptorSet2.Update(
                fragDataBuffer.GetDescriptor(),
                1u,                               // descriptor index
                0u,                               // binding
                0u,                               // array element
                1u,                               // array count
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER // type
            );

            editorDescriptorSet2.Update(
                lightBuffer.GetDescriptor(),
                1u,                               // descriptor index
                1u,                               // binding
                0u,                               // array element
                1u,                               // array count
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER // type
            );

            editorDescriptorSet2.Update(
                shadowMap.descriptor,
                1u,                                       // descriptor index
                2u,                                       // binding
                0u,                                       // array element
                1u,                                       // array count
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
            );
            // TODO: Texture!!
            editorDescriptorSet2.Update(
                testTexture.GetDescriptor(),
                1u,                                       // descriptor index
                3u,                                       // binding
                0u,                                       // array element
                1u,                                       // array count
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // type
            );
        }
    }

    void InitializeFramebuffers() {
        for (std::size_t i{}; i != swapchain.GetImageViewCount(); ++i) {
            VkImageView viewAttachments[]{
                swapchain.GetImageView()[i],
                swapchain.GetDepthBuffer().GetImageView(),

            };
            swapchainFramebuffers.EmplaceBack().Create(renderPass, std::size(viewAttachments), viewAttachments, swapchain.GetExtent(), 1u);
        }
    }

    void InitializeSyncData() {
        auto info{ initializers::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT) };
        fence2.Resize(swapchain.GetImageViewCount());
        for (int i{}; i != swapchain.GetImageViewCount(); ++i) {
            ADH_THROW(vkCreateFence(Context::Get()->GetDevice(), &info, nullptr, &fence1.EmplaceBack()) == VK_SUCCESS,
                      "Failed to create fence!");
        }

        auto info2{ initializers::SemaphoreCreateInfo() };
        for (std::size_t i{}; i != swapchain.GetImageViewCount(); ++i) {
            ADH_THROW(vkCreateSemaphore(Context::Get()->GetDevice(), &info2, nullptr, &presentSempahore.EmplaceBack()) == VK_SUCCESS,
                      "Failed to create semaphore!");
            ADH_THROW(vkCreateSemaphore(Context::Get()->GetDevice(), &info2, nullptr, &renderSemaphore.EmplaceBack()) == VK_SUCCESS,
                      "Failed to create semaphore!");
        }
    }

    void InitializeScripting() {
        ScriptHandler::input = &input;
        ScriptHandler::scene = &scene;
        ScriptHandler::RegisterBindings();
    }

    void CreateEditor() {
        Attachment attachment;
        attachment.AddDescription(
            VK_FORMAT_B8G8R8A8_UNORM,
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            Attachment::Type::eColor);
        attachment.AddDescription(
            tools::GetSupportedDepthFormat(Context::Get()->GetPhysicalDevice()),
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            Attachment::Type::eDepth);

        // Attachment attachment;
        // attachment.AddDescription(
        //     VK_FORMAT_B8G8R8A8_UNORM,
        //     tools::GetMaxSampleCount(Context::Get()->GetPhysicalDevice()),
        //     VK_ATTACHMENT_LOAD_OP_CLEAR,
        //     VK_ATTACHMENT_STORE_OP_STORE,
        //     VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        //     VK_ATTACHMENT_STORE_OP_DONT_CARE,
        //     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        //     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        //     Attachment::Type::eColor);

        // attachment.AddDescription(
        //     tools::GetSupportedDepthFormat(Context::Get()->GetPhysicalDevice()),
        //     tools::GetMaxSampleCount(Context::Get()->GetPhysicalDevice()),
        //     VK_ATTACHMENT_LOAD_OP_CLEAR,
        //     VK_ATTACHMENT_STORE_OP_DONT_CARE,
        //     VK_ATTACHMENT_LOAD_OP_CLEAR,
        //     VK_ATTACHMENT_STORE_OP_STORE,
        //     VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        //     VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        //     Attachment::Type::eDepth);

        // attachment.AddDescription(
        //     VK_FORMAT_B8G8R8A8_UNORM,
        //     VK_SAMPLE_COUNT_1_BIT,
        //     VK_ATTACHMENT_LOAD_OP_CLEAR,
        //     VK_ATTACHMENT_STORE_OP_STORE,
        //     VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        //     VK_ATTACHMENT_STORE_OP_DONT_CARE,
        //     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        //     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        //     Attachment::Type::eResolve);

        Subpass subpass;
        subpass.AddDescription(VK_PIPELINE_BIND_POINT_GRAPHICS, attachment);
        subpass.AddDependencies(
            VK_SUBPASS_EXTERNAL,
            0u,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
        subpass.AddDependencies(
            0u,
            VK_SUBPASS_EXTERNAL,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT);

        VkRect2D renderArea{
            { 0u, 0u },
            { (uint32_t)window.GetWindowWidth(), (uint32_t)window.GetWindowHeight() }
        };

        Array<VkClearValue> clearValues;
        clearValues.Resize(2);
        clearValues[0].color        = { 0.1f, 0.1f, 0.1, 1.0f };
        clearValues[1].depthStencil = {
            1.0f, // float    depth
            0u    // uint32_t stencil
        };
        // clearValues[2].color = { 0.1f, 0.1f, 0.1, 1.0f };
        editor.CreateRenderPass(attachment, subpass, renderArea, Move(clearValues));

        VertexLayout vertexLayout;
        vertexLayout.AddBinding(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
        vertexLayout.AddAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, ADH_OFFSET(Vertex, position));
        vertexLayout.AddAttribute(1, 0, VK_FORMAT_R32G32B32_SFLOAT, ADH_OFFSET(Vertex, normals));
        vertexLayout.AddAttribute(2, 0, VK_FORMAT_R32G32_SFLOAT, ADH_OFFSET(Vertex, textureCoords));
        vertexLayout.Create();

        Shader shader("pbr.vert", "editor_pbr.frag");

        editor.AddGraphicsPipeline(
            shader,
            vertexLayout,
            pipelineLayout,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            VK_CULL_MODE_BACK_BIT,
            VK_FRONT_FACE_COUNTER_CLOCKWISE,
            VK_SAMPLE_COUNT_1_BIT,
            VK_FALSE,
            0.0f,
            VK_TRUE);

        editor.CreateFramebuffers(swapchain);
        editor.InitOverlay(swapchain, scene, sampler, window.GetScreenWidth(), window.GetScreenHeight());
    }

    void OnResize(WindowEvent* event) {
        swapchain.isValid = false;
        event->isHandled  = true;
    }

    void RecreateSwapchain() {
        swapchain.Destroy();
        swapchainFramebuffers.Clear();

        swapchain.Create(swapchanImageCount, VK_FORMAT_B8G8R8A8_UNORM, VK_PRESENT_MODE_FIFO_KHR);
        InitializeFramebuffers();

        renderPass.UpdateRenderArea({ {}, swapchain.GetExtent() });

        clearFramebuffers      = true;
        clearFramebuffersCount = 0;

        // hdrDraw.Update(hdrBuffer.descriptor);

        currentFrame = 0;
        imageIndex   = 0;

        hdrBuffer.Recreate(swapchain);
        gaussianBlur.Recreate(swapchain, hdrBuffer.descriptor);
        hdrDraw.Update(hdrBuffer.descriptor, gaussianBlur.finalPassdescriptor);

        // hdrDraw.Update(gaussianBlur.finalPassdescriptor);
        // hdrDraw.Create(renderPass, gaussianBlur.brightColor.imageInfo[1]);
    }

    void RecreateEditor() {
        editor.Recreate(swapchain);
        editor.UpdateOverlay(swapchain.GetExtent(), sampler);
    }
};

#if defined(ADH_WINDOWS)
#    include <Windows.h>
#    define ENTRY_POINT INT WINAPI WinMain( \
        _In_ HINSTANCE hInstance,           \
        _In_opt_ HINSTANCE hPrevInstance,   \
        _In_ LPSTR lpCmdLine,               \
        _In_ INT nShowCmd)
#    define ADH_WIN_CONSOLE                                                                                                                                          \
        AllocConsole();                                                                                                                                              \
        FILE* fDummy;                                                                                                                                                \
        freopen_s(&fDummy, "CONOUT$", "w", stdout);                                                                                                                  \
        freopen_s(&fDummy, "CONOUT$", "w", stderr);                                                                                                                  \
        freopen_s(&fDummy, "CONIN$", "r", stdin);                                                                                                                    \
        std::cout.clear();                                                                                                                                           \
        std::clog.clear();                                                                                                                                           \
        std::cerr.clear();                                                                                                                                           \
        std::cin.clear();                                                                                                                                            \
        HANDLE hConOut = CreateFile(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); \
        HANDLE hConIn  = CreateFile(L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);  \
        SetStdHandle(STD_OUTPUT_HANDLE, hConOut);                                                                                                                    \
        SetStdHandle(STD_ERROR_HANDLE, hConOut);                                                                                                                     \
        SetStdHandle(STD_INPUT_HANDLE, hConIn);                                                                                                                      \
        std::wcout.clear();                                                                                                                                          \
        std::wclog.clear();                                                                                                                                          \
        std::wcerr.clear();                                                                                                                                          \
        std::wcin.clear();

#else
#    define ENTRY_POINT int main(int argc, char** argv)
#endif

ENTRY_POINT {
#if defined(ADH_WINDOWS) && defined(ADH_DEBUG)
    ADH_WIN_CONSOLE;
#endif
    try {
        AdHoc adhoc;
        adhoc.Initialize(EXE_PATH);
        adhoc.Run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        throw;
    }
}
