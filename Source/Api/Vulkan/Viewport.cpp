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

#include "Viewport.hpp"
#include "Initializers.hpp"

namespace adh {
    namespace vk {
        Viewport::Viewport(float width, float height, bool flipYAxis) noexcept {
            Create(width, height, flipYAxis);
        }

        Viewport::Viewport(VkExtent2D extent, bool flipYAxis) noexcept {
            Create(extent, flipYAxis);
        }

        void Viewport::Create(float width, float height, bool flipYAxis) noexcept {
            if (flipYAxis) {
                m_Viewport = initializers::Viewport(0.0f, height, width, -height);
            } else {
                m_Viewport = initializers::Viewport(0.0f, 0.0f, width, height);
            }
        }

        void Viewport::Create(VkExtent2D extent, bool flipYAxis) noexcept {
            float width{ static_cast<float>(extent.width) };
            float height{ static_cast<float>(extent.height) };
            if (flipYAxis) {
                m_Viewport = initializers::Viewport(0.0f, height, width, -height);
            } else {
                m_Viewport = initializers::Viewport(0.0f, 0.0f, width, height);
            }
        }

        Viewport::Viewport(const VkViewport& viewport) noexcept {
            m_Viewport = viewport;
        }

        void Viewport::Create(const VkViewport& viewport) noexcept {
            m_Viewport = viewport;
        }

        void Viewport::Update(float width, float height, bool flipYAxis) noexcept {
            Create(width, height, flipYAxis);
        }

        void Viewport::Update(VkExtent2D extent, bool flipYAxis) noexcept {
            Create(extent, flipYAxis);
        }

        void Viewport::Update(const VkViewport& viewport) noexcept {
            Create(viewport);
        }

        void Viewport::Set(VkCommandBuffer commandBuffer) noexcept {
            vkCmdSetViewport(commandBuffer, 0u, 1u, &m_Viewport);
        }
    } // namespace vk
} // namespace adh
