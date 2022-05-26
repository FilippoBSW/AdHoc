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

#include "UIOverlay.hpp"
#include "IconFontCppHeaders/IconFontAwesome5.hpp"
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <ImGuizmo/ImGuizmo.h>
#include <Input/Keycodes.hpp>
#include <Scene/Components.hpp>
#include <Scripting/ScriptHandler.hpp>
#include <Vulkan/Context.hpp>

namespace adh {
    UIOverlay::UIOverlay() noexcept {
    }

    UIOverlay::UIOverlay(float width, float height, float aspectRatioWidth, float aspectRatioHeight, const vk::RenderPass& renderPass,
                         VkQueue queue, std::uint32_t imageViewCount, Scene* scene) {
        Create(width, height, aspectRatioWidth, aspectRatioHeight, renderPass, queue, imageViewCount, scene);
    }

    UIOverlay::~UIOverlay() {
        Event::DestroyListener(m_EventListener);
        ImGui::DestroyContext();
    }

    void UIOverlay::Create(float width, float height, float aspectRatioWidth, float aspectRatioHeight, const vk::RenderPass& renderPass,
                           VkQueue queue, std::uint32_t imageViewCount, Scene* scene) {
        ImGui::CreateContext();
        SetUpDisplaySize(width, height);
        SetUpConfigFlags();
        SetUpKeyBindings();
        SetUpDarkTheme();
        AddFonts();
        SetUpEventCallbacks();

        ImGuiStyle& style    = ImGui::GetStyle();
        style.FrameRounding  = 4;
        style.PopupRounding  = 4;
        style.WindowRounding = 12;
        style.ChildRounding  = 12;
        style.FramePadding.x = 6;
        style.FramePadding.y = 0;

        m_ImGui.Create(renderPass, queue, imageViewCount);

        auto& io{ ImGui::GetIO() };

        m_SettingsPath = vk::Context::Get()->GetDataDirectory() + "Resources/Settings/ImGui.ini";
        io.IniFilename = m_SettingsPath.data();

        m_CurrentScene   = scene;
        m_SelectedEntity = ecs::null_entity;

        m_ViewportAspectRatioWidth = m_ScreenAspectWidth = aspectRatioWidth;
        m_ViewportAspectRatioHeight = m_ScreenAspectHeight = aspectRatioHeight;
        m_ViewportAspectRatio = m_ScreenAspectRatio = aspectRatioWidth / aspectRatioHeight;

        auto path{ vk::Context::Get()->GetDataDirectory() + "Assets/" };
        assetPanel.Create((path.substr(0, path.size() - 1u)).data());

        m_Sampler.Create(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_COMPARE_OP_NEVER);

        m_FolderTexture.Create(
            (vk::Context::Get()->GetDataDirectory() + "Assets/Textures/folder_icon.tga").data(),
            VK_IMAGE_USAGE_SAMPLED_BIT,
            &m_Sampler);

        AddTexture("Folder Icon", m_FolderTexture, m_Sampler);

        m_ItemTexture.Create(
            (vk::Context::Get()->GetDataDirectory() + "Assets/Textures/item.tga").data(),
            VK_IMAGE_USAGE_SAMPLED_BIT,
            &m_Sampler);

        AddTexture("Item Icon", m_ItemTexture, m_Sampler);
    }

    void UIOverlay::AddTexture(const std::string& name, VkImageView imageView, const vk::Sampler& sampler) {
        m_TextureIDs.emplace(name, m_ImGui.AddTexture(name, imageView, sampler));
    }

    void UIOverlay::AddTexture(const std::string& name, const VkImageView* imageView, const vk::Sampler& sampler) {
        m_TextureIDs.emplace(name, m_ImGui.AddTexture(name, imageView, sampler));
    }

    void UIOverlay::UpdateTexture(const std::string& name, VkImageView imageView, const vk::Sampler& sampler) {
        m_ImGui.UpdateTexture(name, imageView, sampler);
    }

    void UIOverlay::UpdateTexture(const std::string& name, const VkImageView* imageView, const vk::Sampler& sampler) {
        m_ImGui.UpdateTexture(name, imageView, sampler);
    }

    void UIOverlay::OnUpdate(Scene* scene, float delta, bool drawEditor) {
        m_CurrentScene = scene;
        ImGuiIO& io{ ImGui::GetIO() };
        io.DeltaTime = delta;
        m_DrawEditor = drawEditor;
        m_Keyboard.OnUpdate();
    }

    bool UIOverlay::GetKeyDown(std::uint64_t keycode) noexcept {
        if (m_Keyboard.GetKeyState(keycode) == KeyboardEvent::Type::eKeyDown) {
            return true;
        }

        return false;
    }

    float UIOverlay::GetSelectedAspectRatio() const noexcept {
        return m_ViewportAspectRatio;
    }

    float UIOverlay::GetSelectedAspectRatioWidth() const noexcept {
        return m_ViewportAspectRatioWidth;
    }

    float UIOverlay::GetSelectedAspectRatioHeight() const noexcept {
        return m_ViewportAspectRatioHeight;
    }

    void UIOverlay::Draw(VkCommandBuffer commandBuffer, std::uint32_t imageIndex,
                         bool* maximizeOnPlay, bool* play, bool* pause, bool* fpsLimit, float* floats[]) noexcept {
        NewFrame(maximizeOnPlay, play, pause, fpsLimit, floats);
        m_ImGui.Draw(commandBuffer, imageIndex);
    }

    void UIOverlay::SetUpDisplaySize(float width, float height) const noexcept {
        ImGuiIO& io{ ImGui::GetIO() };
        io.DisplaySize = ImVec2(width, height);
    }

    void UIOverlay::BeginDockSpace() noexcept {
        static ImGuiDockNodeFlags dockspaceFlags{ ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton };

        ImGuiWindowFlags window_flags{ ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking };

        const ImGuiViewport* viewport{ ImGui::GetMainViewport() };
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        static bool dockEnable{ true };
        ImGui::Begin("Dockspace", &dockEnable, window_flags);
        ImGui::PopStyleColor(1);
        ImGui::PopStyleVar(3);

        auto& io{ ImGui::GetIO() };

        ImGuiID dockspaceId{ ImGui::GetID("MyDockSpace") };

        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
    }

    void UIOverlay::EndDockSpace() noexcept {
        ImGui::End();
    }
    static bool settingsOpen{ true };

    void UIOverlay::MenuBar(bool* maximizeOnPlay, bool* play, bool* pause) {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
                                        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground;

        float height = ImGui::GetFrameHeight();

        static bool newScene{ false };
        static bool saveScene{ false };
        static bool saveAsScene{ false };

        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f)); // Menu bar background color
        if (ImGui::BeginViewportSideBar("##SecondaryMenuBar", NULL, ImGuiDir_Up, height, window_flags)) {
            if (ImGui::BeginMenuBar()) {

                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("New", "Ctrl+N")) {
                        Event::Dispatch<StatusEvent>(StatusEvent::Type::eStop);
                        m_CurrentScene->SetTag("Untitled");
                        m_CurrentScene->GetWorld().Reset();
                        m_CurrentScene->GetPhysics().Destroy();
                        m_CurrentScene->GetPhysics().Create();
                        m_CurrentScene->GetState() = lua::NewState();
                        ScriptHandler::scene       = m_CurrentScene;
                        ScriptHandler::RegisterBindings();

                        auto runtimeCamera = m_CurrentScene->GetWorld().CreateEntity();
                        m_CurrentScene->GetWorld().Add<Tag>(runtimeCamera, "Runtime Camera");
                        auto [c2]          = m_CurrentScene->GetWorld().Add<Camera3D>(runtimeCamera, Camera3D{});
                        c2.isRuntimeCamera = true;
                        c2.eyePosition.z   = -8;

                        auto sceneCamera = m_CurrentScene->GetWorld().CreateEntity();
                        m_CurrentScene->GetWorld().Add<Tag>(sceneCamera, "Main Camera");
                        auto [c1]        = m_CurrentScene->GetWorld().Add<Camera3D>(sceneCamera, Camera3D{});
                        c1.isSceneCamera = true;
                        c1.eyePosition.z = -8;
                    }

                    if (ImGui::MenuItem("Open...", "Ctrl+O")) {
#if defined(ADH_APPLE)
                        std::string cmd{ "open " + vk::Context::Get()->GetDataDirectory() + "Assets/Scenes" };
                        system(cmd.data());
#elif defined(ADH_WINDOWS)
                        std::string cmd{ "start " + vk::Context::Get()->GetDataDirectory() + "Assets/Scenes" };
                        for (int i{}; i != cmd.size(); ++i) {
                            if (cmd[i] == '/') {
                                cmd[i] = '\\';
                            }
                        }
                        system(cmd.data());
#endif
                    }

                    if (ImGui::MenuItem("Save", "Ctrl+S")) {
                        saveScene = true;
                    }

                    if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
                        saveAsScene = true;
                    }

                    if (ImGui::MenuItem("Settings")) {
                        settingsOpen = true;
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("View")) {
                    if (ImGui::MenuItem("Hierarchy")) {
                        sceneHierarchyPanel.isOpen = true;
                    }

                    if (ImGui::MenuItem("Inspector")) {
                        inspectorPanel.isOpen = true;
                    }

                    if (ImGui::MenuItem("Game")) {
                        gamePanel.isOpen = true;
                    }

                    if (ImGui::MenuItem("Scene")) {
                        scenePanel.isOpen = true;
                    }

                    if (ImGui::MenuItem("Assets")) {
                        assetPanel.isOpen = true;
                    }

                    if (ImGui::MenuItem("Console")) {
                        consolePanel.isOpen = true;
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Assets")) {
                    if (ImGui::MenuItem("Import")) {
                        system("explorer");
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            ImGui::End();
            ImGui::PopStyleColor(1);
        }

        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        if (ImGui::BeginViewportSideBar("##MainStatusBar", NULL, ImGuiDir_Up, height / 8, window_flags)) {
            if (ImGui::BeginMenuBar()) {

                if (*play || *pause) {
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10, 10 });
                    if (ImGui::Button("Stop")) {
                        Event::Dispatch<StatusEvent>(StatusEvent::Type::eStop);
                    }

                    ImGui::PopStyleVar(1);

                } else if (!(*play) && !(*pause)) {
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10, 10 });
                    if (ImGui::Button("Run")) {
                        Event::Dispatch<StatusEvent>(StatusEvent::Type::eRun);
                    }

                    ImGui::PopStyleVar(1);
                }

                if (*play && !(*pause)) {
                    if (ImGui::Button("Pause")) {
                        Event::Dispatch<StatusEvent>(StatusEvent::Type::ePause);
                    }
                } else if (*play && *pause) {
                    if (ImGui::Button("Unpause")) {
                        Event::Dispatch<StatusEvent>(StatusEvent::Type::eUnpause);
                    }
                }

                ImGui::Checkbox("Maximize on Play", maximizeOnPlay);

                if (m_SelectedEntity != ecs::null_entity) {
                    if (ImGui::Button("Translate")) {
                        guizmoMode = GuizmoMode::eTranslate;
                    }
                    if (ImGui::Button("Rotate")) {
                        guizmoMode = GuizmoMode::eRotate;
                    }
                    if (ImGui::Button("Scale")) {
                        guizmoMode = GuizmoMode::eScale;
                    }
                }

                ImGui::EndMenuBar();
            }

            ImGui::End();
            ImGui::PopStyleColor(1);
        }

        if (saveScene) {
            std::string path{ vk::Context::Get()->GetDataDirectory() + "Assets/Scenes/" + m_CurrentScene->GetTag() + ".scene" };
            m_CurrentScene->SaveToFile(path.data());
            saveScene = false;
        }

        if (saveAsScene) {
            ImGui::OpenPopup("Save as...");
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Save as...", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Separator();
                static char buffer[256];
                std::string buf;
                ImGui::InputText("##Tag", buffer, 256u);
                buf = buffer;
                if (ImGui::Button("Save", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                    saveAsScene = false;
                    std::memset(buffer, 0, 256);

                    std::string path{ vk::Context::Get()->GetDataDirectory() + "Assets/Scenes/" + buf + ".scene" };
                    m_CurrentScene->SetTag(buf);
                    m_CurrentScene->SaveToFile(path.data());
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                    saveAsScene = false;
                }
                ImGui::EndPopup();
            }
        }
    }

    void UIOverlay::NewFrame(bool* maximizeOnPlay, bool* play, bool* pause, bool* fpsLimit, float* floats[]) noexcept {
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
        BeginDockSpace();
        MenuBar(maximizeOnPlay, play, pause);

        gamePanel.Draw(m_TextureIDs["Game Viewport"], m_ViewportAspectRatio);
        scenePanel.Draw(m_CurrentScene, m_SelectedEntity, guizmoMode, m_TextureIDs["Scene Viewport"], m_ViewportAspectRatio);
        assetPanel.Draw(m_CurrentScene, m_TextureIDs["Folder Icon"], m_TextureIDs["Item Icon"]);
        consolePanel.Draw();
        inspectorPanel.Draw(m_CurrentScene, m_SelectedEntity);
        sceneHierarchyPanel.Draw(m_CurrentScene, m_SelectedEntity);

        if (settingsOpen) {
            if (!ImGui::Begin("Setting", &settingsOpen)) {
                ImGui::End();
            } else {
                auto& io = ImGui::GetIO();
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

                ImGui::Checkbox("Editor fps limit", fpsLimit);

                ImGui::SliderFloat("Inteisty", floats[0], 0.0, 20.0);
                ImGui::SliderFloat("Threshold", floats[1], 0.0, 20.0);
                ImGui::SliderFloat("Blur Scale", floats[2], 0.0, 5.0);
                ImGui::SliderFloat("Blur Strengt", floats[3], 0.0, 5.0);
                ImGui::SliderFloat("Light intensity", floats[4], 0.0, 100.0);
                ImGui::SliderFloat("Exposure", floats[5], 0.0, 10.0);

                ImGui::End();
            }
        }

        if (settingsOpen) {
            if (!ImGui::Begin("Setting", &settingsOpen)) {
                ImGui::End();
            } else {
                const char* items[] = { "Screen Aspect", "16/9 Aspect", "16/10 Aspect", "3/2 Aspect", "4/3 Aspect", "9/16 Aspect" };
                static const char* current_item{ items[0] };

                if (ImGui::BeginCombo("Aspect Ratio", current_item)) {
                    for (int n = 0; n < std::size(items); n++) {
                        bool is_selected = (current_item == items[n]);

                        if (ImGui::Selectable(items[n], is_selected)) {
                            current_item = items[n];

                            switch (n) {
                            case 0:
                                m_ViewportAspectRatioWidth  = m_ScreenAspectWidth;
                                m_ViewportAspectRatioHeight = m_ScreenAspectHeight;
                                m_ViewportAspectRatio       = m_ScreenAspectRatio;
                                break;
                            case 1:
                                m_ViewportAspectRatioWidth  = 16.0f;
                                m_ViewportAspectRatioHeight = 9.0f;
                                m_ViewportAspectRatio       = m_ViewportAspectRatioWidth / m_ViewportAspectRatioHeight;
                                break;
                            case 2:
                                m_ViewportAspectRatioWidth  = 16.0f;
                                m_ViewportAspectRatioHeight = 10.0f;
                                m_ViewportAspectRatio       = m_ViewportAspectRatioWidth / m_ViewportAspectRatioHeight;
                                break;
                            case 3:
                                m_ViewportAspectRatioWidth  = 3.0f;
                                m_ViewportAspectRatioHeight = 2.0f;
                                m_ViewportAspectRatio       = m_ViewportAspectRatioWidth / m_ViewportAspectRatioHeight;
                                break;
                            case 4:
                                m_ViewportAspectRatioWidth  = 4.0f;
                                m_ViewportAspectRatioHeight = 3.0f;
                                m_ViewportAspectRatio       = m_ViewportAspectRatioWidth / m_ViewportAspectRatioHeight;
                                break;
                            case 5:
                                m_ViewportAspectRatioWidth  = 9.0f;
                                m_ViewportAspectRatioHeight = 16.0f;
                                m_ViewportAspectRatio       = m_ViewportAspectRatioWidth / m_ViewportAspectRatioHeight;
                                break;
                            }
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::End();
            }
        }

        EndDockSpace();

        // ImGui::ShowDemoWindow();

        ImGui::Render();
    }

    void UIOverlay::SetUpConfigFlags() const noexcept {
        auto& io{ ImGui::GetIO() };
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        io.BackendRendererName = "ImGuiVulkan";
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
        io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
    }

    void UIOverlay::SetUpKeyBindings() const noexcept {
        ImGuiIO& io{ ImGui::GetIO() };
        io.KeyMap[ImGuiKey_Tab]         = ADH_TAB;
        io.KeyMap[ImGuiKey_LeftArrow]   = ADH_LEFT;
        io.KeyMap[ImGuiKey_RightArrow]  = ADH_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow]     = ADH_UP;
        io.KeyMap[ImGuiKey_DownArrow]   = ADH_DOWN;
        io.KeyMap[ImGuiKey_PageUp]      = ADH_PRIOR;
        io.KeyMap[ImGuiKey_PageDown]    = ADH_NEXT;
        io.KeyMap[ImGuiKey_Home]        = ADH_HOME;
        io.KeyMap[ImGuiKey_End]         = ADH_END;
        io.KeyMap[ImGuiKey_Insert]      = ADH_INSERT;
        io.KeyMap[ImGuiKey_Delete]      = ADH_DELETE;
        io.KeyMap[ImGuiKey_Backspace]   = ADH_BACK;
        io.KeyMap[ImGuiKey_Space]       = ADH_SPACE;
        io.KeyMap[ImGuiKey_Enter]       = ADH_RETURN;
        io.KeyMap[ImGuiKey_Escape]      = ADH_ESCAPE;
        io.KeyMap[ImGuiKey_KeyPadEnter] = ADH_RETURN;
        io.KeyMap[ImGuiKey_A]           = ADH_KEY_A;
        io.KeyMap[ImGuiKey_C]           = ADH_KEY_C;
        io.KeyMap[ImGuiKey_V]           = ADH_KEY_V;
        io.KeyMap[ImGuiKey_X]           = ADH_KEY_X;
        io.KeyMap[ImGuiKey_Y]           = ADH_KEY_Y;
        io.KeyMap[ImGuiKey_Z]           = ADH_KEY_Z;
    }

    void UIOverlay::SetUpDarkTheme() const noexcept {
        ImGui::StyleColorsDark();
        auto& colors{ ImGui::GetStyle().Colors }; // #TODO use imcolor
        colors[ImGuiCol_WindowBg]           = ImColor(31, 31, 31);
        colors[ImGuiCol_Header]             = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_HeaderHovered]      = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_HeaderActive]       = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_Button]             = ImColor(40, 40, 40);
        colors[ImGuiCol_ButtonHovered]      = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_ButtonActive]       = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_FrameBg]            = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_FrameBgHovered]     = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_FrameBgActive]      = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_Tab]                = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabHovered]         = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
        colors[ImGuiCol_TabActive]          = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
        colors[ImGuiCol_TabUnfocused]       = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_TitleBg]            = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgActive]      = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed]   = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_MenuBarBg]          = ImColor(40, 40, 40);
        colors[ImGuiCol_Header]             = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_CheckMark]          = ImVec4{ 0.6f, 0.6f, 0.6f, 1.0f };
    }

    void UIOverlay::SetUpEventCallbacks() {
        m_EventListener = Event::CreateListener();
        Event::AddListener<MouseMoveEvent>(m_EventListener, &UIOverlay::OnMouseMoveEvent, this);
        Event::AddListener<MouseButtonEvent>(m_EventListener, &UIOverlay::OnMouseButtonEvent, this);
        Event::AddListener<MouseWheelEvent>(m_EventListener, &UIOverlay::OnMouseWheelEvent, this);
        Event::AddListener<KeyboardEvent>(m_EventListener, &UIOverlay::OnKeyboardEvent, this);
        Event::AddListener<CharEvent>(m_EventListener, &UIOverlay::OnCharEvent, this);
        Event::AddListener<WindowEvent>(m_EventListener, &UIOverlay::OnWindowEvent, this);
        Event::AddListener<WindowDropEvent>(m_EventListener, &UIOverlay::OnWindowDropEvent, this);
    }

    void UIOverlay::AddFonts() noexcept {
        auto& io{ ImGui::GetIO() };
        io.Fonts->AddFontDefault();
        io.FontDefault = io.Fonts->AddFontFromFileTTF((vk::Context::Get()->GetDataDirectory() + "Resources/Fonts/OpenSans/OpenSans-Regular.ttf").data(), 16.0f);
        m_FontIDs.emplace("OpenSansRegular", m_FontIDs.size() + 1u);

        static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        ImFontConfig icons_config;
        icons_config.MergeMode  = true;
        icons_config.PixelSnapH = true;
        io.Fonts->AddFontFromFileTTF((vk::Context::Get()->GetDataDirectory() + "Resources/Fonts/FontAwesome/fa-solid-900.ttf").data(), 16.0f, &icons_config, icons_ranges);

        io.Fonts->AddFontFromFileTTF((vk::Context::Get()->GetDataDirectory() + "Resources/Fonts/OpenSans/OpenSans-Bold.ttf").data(), 16.0f);
        m_FontIDs.emplace("OpenSansBold", m_FontIDs.size() + 1u);

        io.Fonts->AddFontFromFileTTF((vk::Context::Get()->GetDataDirectory() + "Resources/Fonts/OpenSans/OpenSans-Italic.ttf").data(), 16.0f);
        m_FontIDs.emplace("OpenSansItalic", m_FontIDs.size() + 1u);
    }

    void UIOverlay::OnKeyboardEvent(KeyboardEvent* event) noexcept {
        m_Keyboard.SetKeyState(event);

        if (m_DrawEditor) {
            auto& io{ ImGui::GetIO() };

            switch (event->type) {
            case KeyboardEvent::Type::eKeyDown:
                io.KeysDown[event->keycode] = true;
                break;
            case KeyboardEvent::Type::eKeyUp:
                io.KeysDown[event->keycode] = false;
                break;
            }

            if (io.WantCaptureKeyboard) {
                if (!scenePanel.isFocused && !gamePanel.isFocused) {
                    event->isHandled = true;
                }
            }
        }
    }

    void UIOverlay::OnCharEvent(CharEvent* event) noexcept {
        if (m_DrawEditor) {
            auto& io{ ImGui::GetIO() };

            io.AddInputCharacterUTF16(event->keycode);

            if (io.WantTextInput) {
                event->isHandled = true;
            }
        }
    }

    void UIOverlay::OnMouseMoveEvent(MouseMoveEvent* event) noexcept {
        if (m_DrawEditor) {
            auto& io{ ImGui::GetIO() };
            io.MousePos = ImVec2(event->x, event->y);

            if (io.WantCaptureMouse) {
                if (!scenePanel.rect.IsInViewportRect(event->x, event->y) &&
                    !gamePanel.rect.IsInViewportRect(event->x, event->y)) {
                    event->isHandled = true;
                }
            }

            event->x -= scenePanel.rect.left;
            event->y -= scenePanel.rect.top;
        }
    }

    void UIOverlay::OnMouseButtonEvent(MouseButtonEvent* event) noexcept {
        if (m_DrawEditor) {
            auto& io{ ImGui::GetIO() };

            switch (event->type) {
            case MouseButtonEvent::Type::eLeftButtonDown:
                io.MouseDown[static_cast<int>(event->index)] = true;
                break;
            case MouseButtonEvent::Type::eLeftButtonUp:
                io.MouseDown[static_cast<int>(event->index)] = false;
                break;
            case MouseButtonEvent::Type::eRightButtonDown:
                io.MouseDown[static_cast<int>(event->index)] = true;
                break;
            case MouseButtonEvent::Type::eRightButtonUp:
                io.MouseDown[static_cast<int>(event->index)] = false;
                break;
            case MouseButtonEvent::Type::eMiddleButtonDown:
                io.MouseDown[static_cast<int>(event->index)] = true;
                break;
            case MouseButtonEvent::Type::eMiddleButtonUp:
                io.MouseDown[static_cast<int>(event->index)] = false;
                break;
            }

            if (io.WantCaptureMouse) {
                if (!scenePanel.rect.IsInViewportRect(event->x, event->y) &&
                    !gamePanel.rect.IsInViewportRect(event->x, event->y)) {
                    event->isHandled = true;
                }
            }
        }
    }

    void UIOverlay::OnMouseWheelEvent(MouseWheelEvent* event) noexcept {
        if (m_DrawEditor) {
            auto& io{ ImGui::GetIO() };
            io.MouseWheel += event->delta;
            if (io.WantCaptureMouse) {
                if (!scenePanel.rect.IsInViewportRect(event->x, event->y) &&
                    !gamePanel.rect.IsInViewportRect(event->x, event->y)) {
                    event->isHandled = true;
                }
            }
        }
    }

    void UIOverlay::OnWindowEvent(WindowEvent* event) noexcept {
        auto& io{ ImGui::GetIO() };
        if (event->type == WindowEvent::Type::eKillfocus) {
            std::memset(io.KeysDown, 0, sizeof(io.KeysDown));
            m_Keyboard.OnKillFocus();
        }
    }

    // TODO:
    void UIOverlay::OnWindowDropEvent(WindowDropEvent* event) noexcept {
        //         std::string path{ event->filePath };

        //         auto namePos{ path.find_last_of("\\") };
        //         std::string fileName{ path.substr(namePos + 1u) };

        //         auto typePos{ fileName.find_last_of(".") };
        //         std::string fileType{ fileName.substr(typePos + 1u) };

        //         if (fileType == "tga") {
        //             std::string copyPath{ vk::Context::Get()->GetDataDirectory() + "Assets/Textures/" + fileName };

        // #if defined(ADH_WINDOWS)
        //             CopyFileA(event->filePath, copyPath.data(), TRUE);
        // #endif

        //             event->isHandled = true;
        //         }

        //         else if (fileType == "obj" ||
        //                  fileType == "glb" ||
        //                  fileType == "fbx" ||
        //                  fileType == "ply") {
        //             std::string copyPath{ vk::Context::Get()->GetDataDirectory() + "Assets/Models/" + fileName };

        // #if defined(ADH_WINDOWS)
        //             CopyFileA(event->filePath, copyPath.data(), TRUE);
        // #elif defined(ADH_APPLE)
        //             // copyfile(event->filePath, copyPath.data());
        // #endif

        //             event->isHandled = true;
        //         }

        //         else {
        //             // #TODO log non supported format
        //         }
    }
} // namespace adh
