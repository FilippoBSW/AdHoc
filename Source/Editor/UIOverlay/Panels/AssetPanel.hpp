#pragma once
#include <filesystem>

namespace adh {
    class Scene;
    class AssetPanel {
        using FilePath = std::filesystem::path;

      public:
        AssetPanel() noexcept = default;

        AssetPanel(const char* assetPath);

        void Create(const char* assetPath);

        void Draw(Scene* scene, void* folderIconID, void* itemIconID) noexcept;

      public:
        FilePath dataDirectory;
        FilePath currentDirectory;
        bool isOpen{ true };
    };
} // namespace adh
