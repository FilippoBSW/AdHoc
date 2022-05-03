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

#include "Vulkan/Attachments.hpp"
#include "Vulkan/Subpass.hpp"
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
#include <Vulkan/CommandBuffer.hpp>
#include <Vulkan/CommandPool.hpp>
#include <Vulkan/Context.hpp>
#include <Vulkan/DescriptorSet.hpp>
#include <Vulkan/GraphicsPipeline.hpp>
#include <Vulkan/IndexBuffer.hpp>
#include <Vulkan/PipelineLayout.hpp>
#include <Vulkan/RenderPass.hpp>
#include <Vulkan/Sampler.hpp>
#include <Vulkan/Scissor.hpp>
#include <Vulkan/Shader.hpp>
#include <Vulkan/Swapchain.hpp>
#include <Vulkan/Tools.hpp>
#include <Vulkan/UniformBuffer.hpp>
#include <Vulkan/VertexBuffer.hpp>
#include <Vulkan/VertexLayout.hpp>
#include <Vulkan/Viewport.hpp>
#include <Window.hpp>

#include <Audio/Audio.hpp>

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

Vector3D sunPosition{ 6.0f, 10.0f, -10.0f };

struct FragmentData {
    alignas(16) Vector3D ambient;
    alignas(16) Vector3D cameraPosition;
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
                                    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_TRUE);

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

        m_Extent.width  = 2048;
        m_Extent.height = 2048;

        m_Image.Create(
            { m_Extent.width, m_Extent.height, 1u },
            tools::GetSupportedDepthFormat(Context::Get()->GetPhysicalDevice()),
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_TYPE_2D,
            (VkImageCreateFlagBits)0,
            1,
            1,
            VkImageUsageFlagBits(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT),
            VK_IMAGE_ASPECT_DEPTH_BIT,
            VK_IMAGE_VIEW_TYPE_2D,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            VK_SHARING_MODE_EXCLUSIVE);

        Attachment attachment;
        attachment.AddDescription(
            tools::GetSupportedDepthFormat(Context::Get()->GetPhysicalDevice()),
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

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

        VkImageView viewAttachments[]{
            m_Image.GetImageView()
        };

        auto info{ initializers::FramebufferCreateInfo(m_RenderPass, std::size(viewAttachments), viewAttachments, m_Extent, 1u) };
        ADH_THROW(vkCreateFramebuffer(Context::Get()->GetDevice(), &info, nullptr, &m_Framebuffer) == VK_SUCCESS,
                  "Failed to create frame buffers!");

        Shader shader("shadowmap.vert", "shadowmap.frag");

        VertexLayout vertexLayout;
        vertexLayout.AddBinding(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
        vertexLayout.AddAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, ADH_OFFSET(Vertex, position));
        vertexLayout.AddAttribute(1, 0, VK_FORMAT_R32G32B32_SFLOAT, ADH_OFFSET(Vertex, normals));
        vertexLayout.Create();

        pipelineLayout.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
        pipelineLayout.CreateSet();

        pipelineLayout.AddPushConstant(VK_SHADER_STAGE_VERTEX_BIT, sizeof(xmm::Matrix), 0);
        pipelineLayout.Create();

        graphicsPipeline.Create(shader, vertexLayout, pipelineLayout, m_RenderPass, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_TRUE);

        descriptorSet.Initialize(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 3);
        descriptorSet.AddPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
        descriptorSet.Create(pipelineLayout.GetSetLayout());

        lightSpace = xmm::PerspectiveLH(ToRadians(140.0f), 1.0f, 1.0f, 1000.0f) * xmm::LookAtLH(sunPosition, { 0, 0, 0 }, { 0, 1, 0 });

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

        descriptor.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        descriptor.sampler     = sampler;
        descriptor.imageView   = m_Image.GetImageView();

        debug.Create(renderPass, descriptor);
    }

    RenderPass m_RenderPass;
    VkExtent2D m_Extent;
    vk::Image m_Image;
    VkFramebuffer m_Framebuffer;

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

class AdHoc {
  public:
    const char* name{ "AdHoc" };
    Window window;
    Context context;
    Swapchain swapchain;
    std::uint32_t swapchanImageCount{ 3 };

    Sampler sampler;
    Scene scene;
    Editor editor;
    Array<VkFramebuffer> swapchainFramebuffers;
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
    bool g_MaximizeOnPlay{ false };
    bool maximizeOnPlay2{ false };

    bool g_IsPlaying{ false };
    bool g_IsPaused{ false };
    bool g_AreScriptsReady{ false };

    ShadowMap2D shadowMap;

  public:
    ~AdHoc() {
        auto device{ Context::Get()->GetDevice() };
        vkDeviceWaitIdle(device);
        vkDestroyFramebuffer(device, shadowMap.m_Framebuffer, nullptr);
        for (int i{}; i != swapchain.GetImageViewCount(); ++i) {
            vkDestroyFramebuffer(device, swapchainFramebuffers[i], nullptr);
            vkDestroySemaphore(device, presentSempahore[i], nullptr);
            vkDestroySemaphore(device, renderSemaphore[i], nullptr);
            vkDestroyFence(device, fence1[i], nullptr);
        }
        Mesh::Clear(); // TODO: temp
    }

    void OnCollisionEvent(CollisionEvent* event) {
        if (g_IsPlaying && g_AreScriptsReady) {
            scene.GetWorld().GetSystem<lua::Script>().ForEach([&](ecs::Entity ent, lua::Script& script) {
                bool call{};
                EntityID rhs{};
                EntityID e = static_cast<EntityID>(ent);

                if (e == event->entityA) {
                    rhs  = event->entityB;
                    call = true;
                } else if (e == event->entityB) {
                    rhs  = event->entityA;
                    call = true;
                }

                if (call && script.collisionEvent.type == CollisionEvent::Type::eCollisionInvalid) {
                    script.collisionEvent = { event->type, 0, rhs };
                }
            });
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

    void Initialize(const char* path) {
        try {
            window.Create(name, 1200, 800, true, false);
            context.Create(window, "AdHoc", path);
            swapchain.Create(swapchanImageCount, VK_FORMAT_B8G8R8A8_UNORM, VK_PRESENT_MODE_MAILBOX_KHR);
            InitializeRenderPass();

            sampler.Create(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_COMPARE_OP_NEVER, VK_FALSE, VK_TRUE);
            shadowMap.Create(renderPass, sampler);

            lightSpace = lightSpaceBias * shadowMap.lightSpace;

            InitializePipeline();
            InitializeDescriptorSets();
            InitializeEditorDescriptorSets();
            commandBuffer.Create(VK_COMMAND_BUFFER_LEVEL_PRIMARY, swapchain.GetImageViewCount(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, DeviceQueues::Family::eGraphics);
            InitializeFramebuffers();
            InitializeSyncData();

            InitializeScripting();
            CreateEditor();
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            throw;
        }

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

        renderingReady = true;
    }

    void Run() {
        Stopwatch timer;
        float deltaTime;

        while (window.IsOpen()) {
            if (!renderingReady) {
                continue;
            }

            while (window.IsMinimized()) {
                window.PollEvents();
            }

            deltaTime                = timer.Lap();
            ScriptHandler::deltaTime = deltaTime;
            editor.OnUpdate(&scene, deltaTime, g_DrawEditor);
            input.OnUpdate();
            input.PollEvents();
            window.PollEvents();

            UpdateCameras();
            UpdateScripts(deltaTime);

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

            if (currentFrame % swapchain.GetImageViewCount()) {
                if (ScriptHandler::loadSceneFilename) {
                    scene.LoadFromFile((Context::Get()->GetDataDirectory() + "Assets/Scenes/" + ScriptHandler::loadSceneFilename).data());
                    ScriptHandler::loadSceneFilename = nullptr;
                    scene.GetState().ClearStack();
                    scene.ResetPhysicsWorld();
                    ReadyScript();
                }
                if (!ScriptHandler::toDestroy.IsEmpty()) {
                    for (auto&& i : ScriptHandler::toDestroy) {
                        // scene.GetWorld().Destroy(i);
                        i(&scene);
                    }
                    ScriptHandler::toDestroy.Clear();
                }
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
                switch (script.collisionEvent.type) {
                case CollisionEvent::Type::eCollisionEnter:
                    {
                        script.Call("OnCollisionEnter", script.collisionEvent.entityB);
                        script.collisionEvent = { CollisionEvent::Type::eCollisionInvalid, 0, 0 };
                        break;
                    }
                case CollisionEvent::Type::eCollisionPersist:
                    {
                        script.Call("OnCollisionPersist", script.collisionEvent.entityB);
                        script.collisionEvent = { CollisionEvent::Type::eCollisionInvalid, 0, 0 };
                        break;
                    }
                case CollisionEvent::Type::eCollisionExit:
                    {
                        script.Call("OnCollisionExit", script.collisionEvent.entityB);
                        script.collisionEvent = { CollisionEvent::Type::eCollisionInvalid, 0, 0 };
                        break;
                    }
                case CollisionEvent::Type::eTriggerEnter:
                    {
                        script.Call("OnTriggerEnter", script.collisionEvent.entityB);
                        script.collisionEvent = { CollisionEvent::Type::eCollisionInvalid, 0, 0 };
                        break;
                    }
                case CollisionEvent::Type::eTriggerPersist:
                    {
                        script.Call("OnTriggerPersist", script.collisionEvent.entityB);
                        script.collisionEvent = { CollisionEvent::Type::eCollisionInvalid, 0, 0 };
                        break;
                    }
                case CollisionEvent::Type::eTriggerExit:
                    {
                        script.Call("OnTriggerExit", script.collisionEvent.entityB);
                        script.collisionEvent = { CollisionEvent::Type::eCollisionInvalid, 0, 0 };
                        break;
                    }
                }
                script.Unbind();
            });
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

        // Draw shadowmap
        {
            shadowMap.m_RenderPass.Begin(cmd, shadowMap.m_Framebuffer);
            shadowMap.graphicsPipeline.Bind(cmd);
            shadowMap.descriptorSet.Bind(cmd, 0);
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

        static bool clearFramebuffers;
        static int clearFramebuffersCount;

        if (g_DrawEditor == true) {
            clearFramebuffers = true;
            renderPass.UpdateRenderArea({ {}, swapchain.GetExtent() });
        } else if (!g_DrawEditor && !clearFramebuffers) {
            renderPass.SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            renderPass.UpdateRenderArea(g_AspectRatio.GetRect());
        } else if (!g_DrawEditor && clearFramebuffers) {
            renderPass.SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

            // viewport.Update(shadowMap.m_Extent, true);
            // viewport.Set(cmd);

            // scissor.Update(shadowMap.m_Extent);
            // scissor.Set(cmd);

            // float depthBiasConstant = 1.25f;
            // float depthBiasSlope    = 1.75f;
            // vkCmdSetDepthBias(cmd, depthBiasConstant, 0.0f, depthBiasSlope);

            // vkCmdDraw(cmd, 3, 1, 0, 0);

            viewport.Update(g_AspectRatio.GetViewport());
            viewport.Set(cmd);

            scissor.Update(g_AspectRatio.GetRect());
            scissor.Set(cmd);

            vkCmdSetDepthBias(cmd, 0.0f, 0.0f, 0.0f);

            graphicsPipeline.Bind(cmd);
            descriptorSet.Bind(cmd, imageIndex);

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

        } else {
            editor.Draw(cmd, currentFrame, &g_MaximizeOnPlay, &g_IsPlaying, &g_IsPaused);
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

    void InitializeRenderPass() {
        Attachment attachment;
        attachment.AddDescription(
            VK_FORMAT_B8G8R8A8_UNORM,
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        attachment.AddDescription(
            tools::GetSupportedDepthFormat(Context::Get()->GetPhysicalDevice()),
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

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
        clearValues[0].color        = { 0.1f, 0.1f, 0.1f, 1.0f };
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
        vertexLayout.Create();

        pipelineLayout.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
        pipelineLayout.AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
        pipelineLayout.CreateSet();

        pipelineLayout.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        pipelineLayout.AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        pipelineLayout.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        pipelineLayout.CreateSet();

        pipelineLayout.AddPushConstant(VK_SHADER_STAGE_VERTEX_BIT, sizeof(xmm::Matrix), 0);
        pipelineLayout.AddPushConstant(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(Material), 64);

        pipelineLayout.Create();

        graphicsPipeline.Create(shader, vertexLayout, pipelineLayout, renderPass,
                                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_CULL_MODE_FRONT_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_TRUE);
    }

    void InitializeDescriptorSets() {
        fragmentUbo.ambient        = { 0.1f, 0.1f, 0.1f };
        fragmentUbo.cameraPosition = { 0.0f, 0.0, -8.0f };

        directionalLight.direction = sunPosition;
        directionalLight.color     = { 1.0f, 1.0f, 1.0f };
        directionalLight.intensity = 1.0f;

        descriptorSet.Initialize(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, swapchain.GetImageViewCount());
        descriptorSet.AddPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4);
        descriptorSet.AddPool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
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
    }

    void InitializeEditorDescriptorSets() {
        fragmentUbo.ambient        = { 0.1f, 0.1f, 0.1f };
        fragmentUbo.cameraPosition = { 0.0f, 0.0, -8.0f };
        directionalLight.direction = sunPosition;
        directionalLight.color     = { 1.0f, 1.0f, 1.0f };
        directionalLight.intensity = 1.0f;
        {
            editorDescriptorSet.Initialize(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, swapchain.GetImageViewCount());
            editorDescriptorSet.AddPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4);
            editorDescriptorSet.AddPool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
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
        }
        {
            editorDescriptorSet2.Initialize(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, swapchain.GetImageViewCount());
            editorDescriptorSet2.AddPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4);
            editorDescriptorSet2.AddPool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
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
        }
    }

    void InitializeFramebuffers() {
        for (std::size_t i{}; i != swapchain.GetImageViewCount(); ++i) {
            VkImageView viewAttachments[]{
                swapchain.GetImageView()[i],
                swapchain.GetDepthBuffer().GetImageView()
            };
            auto info{ initializers::FramebufferCreateInfo(renderPass, std::size(viewAttachments), viewAttachments, swapchain.GetExtent(), 1u) };
            ADH_THROW(vkCreateFramebuffer(Context::Get()->GetDevice(), &info, nullptr, &swapchainFramebuffers.EmplaceBack()) == VK_SUCCESS,
                      "Failed to create frame buffers!");
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
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        attachment.AddDescription(
            tools::GetSupportedDepthFormat(Context::Get()->GetPhysicalDevice()),
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

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
        editor.CreateRenderPass(attachment, subpass, renderArea, Move(clearValues));

        VertexLayout vertexLayout;
        vertexLayout.AddBinding(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
        vertexLayout.AddAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, ADH_OFFSET(Vertex, position));
        vertexLayout.AddAttribute(1, 0, VK_FORMAT_R32G32B32_SFLOAT, ADH_OFFSET(Vertex, normals));
        vertexLayout.Create();

        Shader shader("pbr.vert", "pbr.frag");

        editor.AddGraphicsPipeline(
            shader,
            vertexLayout,
            pipelineLayout,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            VK_CULL_MODE_BACK_BIT,
            VK_FRONT_FACE_COUNTER_CLOCKWISE,
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
        for (std::size_t i{}; i != swapchainFramebuffers.GetSize(); ++i) {
            vkDestroyFramebuffer(Context::Get()->GetDevice(), swapchainFramebuffers[i], nullptr);
        }
        swapchainFramebuffers.Clear();

        swapchain.Create(swapchanImageCount, VK_FORMAT_B8G8R8A8_UNORM, VK_PRESENT_MODE_MAILBOX_KHR);
        InitializeFramebuffers();

        renderPass.UpdateRenderArea({ {}, swapchain.GetExtent() });
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
    AdHoc adhoc;
    adhoc.Initialize(EXE_PATH);
    adhoc.Run();
}
