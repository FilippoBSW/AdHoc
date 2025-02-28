#pragma once
#include <string>

namespace adh {
    struct Tag {
        Tag() = default;

        Tag(const std::string& tag) : tag{ tag } {
        }

        const char* Get() const noexcept {
            return tag.data();
        }

        void Set(const char* tag) noexcept {
            this->tag = tag;
        }
        std::string tag;
    };
} // namespace adh
