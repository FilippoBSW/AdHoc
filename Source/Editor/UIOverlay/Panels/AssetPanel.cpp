#include "AssetPanel.hpp"
#include "../IconFontCppHeaders/IconFontAwesome5.hpp"
#include <ImGui/imgui.h>

#include <Scene/Scene.hpp>

#if defined(ADH_WINDOWS)
#    include <Windows.h>
#endif

namespace adh {
    AssetPanel::AssetPanel(const char* assetPath) {
        Create(assetPath);
    }

    void AssetPanel::Create(const char* assetPath) {
        currentDirectory = dataDirectory = assetPath;
    }

    void AssetPanel::Draw(Scene* scene, void* folderIconID, void* itemIconID) noexcept {
        if (isOpen) {
            if (!ImGui::Begin("Assets", &isOpen)) {
                ImGui::End();
            }

            else {
                if (currentDirectory != dataDirectory) {
                    if (ImGui::Button(ICON_FA_LONG_ARROW_ALT_LEFT)) {
                        currentDirectory = currentDirectory.parent_path();
                    }
                }

                float padding{ 8.0f };
                float thumbnailSize{ 64.0f };
                float cellSize{ thumbnailSize + padding };
                float panelWidth{ ImGui::GetContentRegionAvail().x };
                auto columnCount{ static_cast<std::int32_t>(std::max(panelWidth / cellSize, 1.0f)) };

                ImGui::Columns(columnCount, nullptr, false);

                for (auto&& directoryEntry : std::filesystem::directory_iterator(currentDirectory)) {
                    std::string dir = directoryEntry.path().string();
                    auto pos        = dir.find_last_of('/');
                    auto str        = dir.substr(pos + 1);
                    if (str[0] != '.') {
                        ImGui::PushID(directoryEntry.path().string().data());
                        const auto& path{ directoryEntry.path() };
                        auto relativePath{ std::filesystem::relative(path, currentDirectory) };
                        auto filenameString{ relativePath.filename().string() };

                        if (directoryEntry.is_directory()) {
                            ImGui::ImageButton(folderIconID, { thumbnailSize, thumbnailSize }, ImVec2(0, 1), ImVec2(1, 0));
                        }

                        else {
                            ImGui::ImageButton(itemIconID, { thumbnailSize, thumbnailSize }, ImVec2(0, 1), ImVec2(1, 0));

                            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                                ImGui::SetDragDropPayload("DND_DEMO_CELL", directoryEntry.path().string().data(), directoryEntry.path().string().size() + 1);
                                ImGui::EndDragDropSource();
                            }
                        }

                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                            if (directoryEntry.is_directory()) {
                                currentDirectory /= path.filename();
                            } else {
#if defined(ADH_APPLE)
                                std::string t = "open " + directoryEntry.path().string();
                                system(t.data());
#elif defined(ADH_LINUX)
                                std::string t = "vim " + directoryEntry.path().string();
                                system(t.data());
#elif defined(ADH_WINDOWS)
                                std::string t = "start " + directoryEntry.path().string();
                                for (int i{}; i != t.size(); ++i) {
                                    if (t[i] == '/') {
                                        t[i] = '\\';
                                    }
                                }
                                system(t.data());
#endif
                            }
                        }

                        if (ImGui::BeginPopupContextItem(directoryEntry.path().string().data())) {
                            if (ImGui::MenuItem("Open")) {
                                Event::Dispatch<StatusEvent>(StatusEvent::Type::eStop);
                                scene->LoadFromFile(directoryEntry.path().string().data());
                            }

                            if (ImGui::MenuItem("Delete")) {
#if defined(ADH_APPLE)
                                std::string del = "rm " + directoryEntry.path().string();
                                system(del.data());
#elif defined(ADH_LINUX)
                                std::string del = "rm " + directoryEntry.path().string();
                                system(del.data());
#elif defined(ADH_WINDOWS)
                                DeleteFileA(directoryEntry.path().string().data());
#endif
                            }

                            ImGui::EndPopup();
                        }

                        ImGui::TextWrapped(filenameString.data());

                        ImGui::NextColumn();
                        ImGui::PopID();
                    }
                }

                ImGui::End();
            }
        }
    }
} // namespace adh
