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

#include "Sampler.hpp"
#include "Context.hpp"
#include "Initializers.hpp"
#include "Tools.hpp"

namespace adh {
    namespace vk {
        Sampler::Sampler() noexcept : m_Sampler{ VK_NULL_HANDLE } {
        }

        Sampler::Sampler(
            VkFilter filter,
            VkSamplerAddressMode mode,
            VkSamplerMipmapMode mipmapMode,
            VkCompareOp compareOp,
            VkBool32 useMipMap,
            VkBool32 anisotropyEnable) ADH_NOEXCEPT {
            Create(filter, mode, mipmapMode, compareOp, useMipMap, anisotropyEnable);
        }

        Sampler::Sampler(Sampler&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        Sampler& Sampler::operator=(Sampler&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));

            return *this;
        }

        Sampler::~Sampler() {
            Clear();
        }

        void Sampler::Create(
            VkFilter filter,
            VkSamplerAddressMode mode,
            VkSamplerMipmapMode mipmapMode,
            VkCompareOp compareOp,
            VkBool32 useMipMap,
            VkBool32 anisotropyEnable) ADH_NOEXCEPT {
            auto physicalDeviceProperties{ tools::GetPhysicalDeviceProperties(Context::Get()->GetPhysicalDevice()) };

            auto info{ initializers::SamplerCreateInfo(
                filter,
                filter,
                mipmapMode,
                mode,
                compareOp,
                useMipMap,
                physicalDeviceProperties.limits.maxSamplerAnisotropy,
                anisotropyEnable) };

            ADH_THROW(vkCreateSampler(Context::Get()->GetDevice(), &info, nullptr, &m_Sampler) == VK_SUCCESS,
                      "Failed to create sampler!");
        }

        VkSampler Sampler::Get() noexcept {
            return m_Sampler;
        }

        const VkSampler Sampler::Get() const noexcept {
            return m_Sampler;
        }

        void Sampler::Destroy() noexcept {
            Clear();
        }

        Sampler::operator VkSampler() noexcept {
            return m_Sampler;
        }

        Sampler::operator const VkSampler() const noexcept {
            return m_Sampler;
        }

        void Sampler::MoveConstruct(Sampler&& rhs) noexcept {
            m_Sampler     = rhs.m_Sampler;
            rhs.m_Sampler = VK_NULL_HANDLE;
        }

        void Sampler::Clear() noexcept {
            if (m_Sampler != VK_NULL_HANDLE) {
                vkDestroySampler(Context::Get()->GetDevice(), m_Sampler, nullptr);
                m_Sampler = VK_NULL_HANDLE;
            }
        }
    } // namespace vk
} // namespace adh
