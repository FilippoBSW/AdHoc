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
#include <string>
#include <unordered_map>

#include <Std/Array.hpp>

#include "../Core/Input/Keyboard.hpp"

#include "Panels/AssetPanel.hpp"
#include "Panels/ConsolePanel.hpp"
#include "Panels/GamePanel.hpp"
#include "Panels/InspectorPanel.hpp"
#include "Panels/SceneHierarchyPanel.hpp"
#include "Panels/ScenePanel.hpp"

#include "../Api/Vulkan/VulkanImGui.hpp"

namespace adh {
    class UIOverlay {
      public:
        using TextureIdMap = std::unordered_map<std::string, void*>;
        using FontIdMap    = std::unordered_map<std::string, std::uint32_t>;

      public:
        UIOverlay() noexcept;

        UIOverlay(float width, float height, float aspectRatioWidth, float aspecRatioHeight, const vk::RenderPass& renderPass,
                  VkQueue queue, std::uint32_t imageViewCount, Scene* scene = nullptr);

        UIOverlay(const UIOverlay& rhs) = delete;

        UIOverlay& operator=(const UIOverlay& rhs) = delete;

        UIOverlay(UIOverlay&& rhs) = delete;

        UIOverlay& operator=(UIOverlay&& rhs) = delete;

        ~UIOverlay();

        void Create(float width, float height, float aspectRatioWidth, float aspecRatioHeight, const vk::RenderPass& renderPass,
                    VkQueue queue, std::uint32_t imageViewCount, Scene* scene = nullptr);

        void AddTexture(const std::string& name, VkImageView imageView, const vk::Sampler& sampler);

        void AddTexture(const std::string& name, const VkImageView* imageView, const vk::Sampler& sampler);

        void UpdateTexture(const std::string& name, VkImageView imageView, const vk::Sampler& sampler);

        void UpdateTexture(const std::string& name, const VkImageView* imageView, const vk::Sampler& sampler);

        void OnUpdate(Scene* scene, float delta, bool drawEditor);

        void Draw(VkCommandBuffer commandBuffer, std::uint32_t imageIndex,
                  bool* maximizeOnPlay, bool* play, bool* pause) noexcept;

        void SetUpDisplaySize(float width, float height) const noexcept;

        bool GetKeyDown(std::uint64_t keycode) noexcept;

        float GetSelectedAspectRatio() const noexcept;

        float GetSelectedAspectRatioWidth() const noexcept;

        float GetSelectedAspectRatioHeight() const noexcept;

      private:
        void BeginDockSpace() noexcept;

        void EndDockSpace() noexcept;

        void MenuBar(bool* drawEditor, bool* play, bool* pause);

        void NewFrame(bool* maximizeOnPlay, bool* play, bool* pause) noexcept;

        void SetUpConfigFlags() const noexcept;

        void SetUpKeyBindings() const noexcept;

        void SetUpDarkTheme() const noexcept;

        void SetUpEventCallbacks();

        void AddFonts() noexcept;

        void OnKeyboardEvent(KeyboardEvent* event) noexcept;

        void OnCharEvent(CharEvent* event) noexcept;

        void OnMouseMoveEvent(MouseMoveEvent* event) noexcept;

        void OnMouseButtonEvent(MouseButtonEvent* event) noexcept;

        void OnMouseWheelEvent(MouseWheelEvent* event) noexcept;

        void OnWindowEvent(WindowEvent* event) noexcept;

        void OnWindowDropEvent(WindowDropEvent* event) noexcept;

      private:
        vk::VulkanImGui m_ImGui;
        TextureIdMap m_TextureIDs;
        FontIdMap m_FontIDs;
        EventListener m_EventListener{};
        std::string m_SettingsPath;

        vk::Texture2D m_FolderTexture;
        vk::Texture2D m_ItemTexture;
        vk::Sampler m_Sampler;

        Scene* m_CurrentScene{};
        ecs::Entity m_SelectedEntity{};

        GamePanel gamePanel;
        ScenePanel scenePanel;
        AssetPanel assetPanel;
        InspectorPanel inspectorPanel;
        SceneHierarchyPanel sceneHierarchyPanel;
        ConsolePanel consolePanel;

        float m_ScreenAspectWidth;
        float m_ScreenAspectHeight;
        float m_ScreenAspectRatio;
        float m_ViewportAspectRatio;
        float m_ViewportAspectRatioWidth;
        float m_ViewportAspectRatioHeight;

        Keyboard m_Keyboard;
        bool m_DrawEditor;

        GuizmoMode guizmoMode{ GuizmoMode::eTranslate };
    };

} // namespace adh
