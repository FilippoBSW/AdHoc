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

#pragma once
#include <Utility.hpp>
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        class Sampler {
          public:
            Sampler() noexcept;

            Sampler(
                VkFilter filter,
                VkSamplerAddressMode mode,
                VkSamplerMipmapMode mipmapMode,
                VkCompareOp compareOp,
                VkBool32 useMipMap        = VK_FALSE,
                VkBool32 anisotropyEnable = VK_FALSE) ADH_NOEXCEPT;

            Sampler(const Sampler& rhs) = delete;

            Sampler& operator=(const Sampler& rhs) = delete;

            Sampler(Sampler&& rhs) noexcept;

            Sampler& operator=(Sampler&& rhs) noexcept;

            ~Sampler();

            void Create(
                VkFilter filter,
                VkSamplerAddressMode mode,
                VkSamplerMipmapMode mipmapMode,
                VkCompareOp compareOp,
                VkBool32 useMipMap        = VK_FALSE,
                VkBool32 anisotropyEnable = VK_FALSE) ADH_NOEXCEPT;

            VkSampler Get() noexcept;

            const VkSampler Get() const noexcept;

            void Destroy() noexcept;

            operator VkSampler() noexcept;

            operator const VkSampler() const noexcept;

          private:
            void MoveConstruct(Sampler&& rhs) noexcept;

            void Clear() noexcept;

          private:
            VkSampler m_Sampler;
        };
    } // namespace vk
} // namespace adh
