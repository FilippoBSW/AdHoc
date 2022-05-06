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

#include "Instance.hpp"
#include "Initializers.hpp"

#include <Std/Array.hpp>
#include <Utility.hpp>

namespace adh {
    namespace vk {
        Instance::Instance() noexcept : m_Instance{ VK_NULL_HANDLE } {
        }

        Instance::Instance(const char* name) {
            Create(name);
        }

        Instance::Instance(Instance&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        Instance& Instance::operator=(Instance&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));
            return *this;
        }

        Instance::~Instance() {
            Clear();
        }

        void Instance::Create(const char* name) {
            auto applicationInfo{ initializers::ApplicationInfo("AdHoc", VK_API_VERSION_1_2) };
            Array<const char*> validationLayers;
#if defined(ADH_DEBUG)
            validationLayers.EmplaceBack("VK_LAYER_KHRONOS_validation");
#endif
            Array<const char*> instanceExtentions{ "VK_KHR_surface", ADH_VK_PLATFORM_SURFACE };
            auto instanceCreateInfo{ initializers::InstanceCreateInfo(applicationInfo, instanceExtentions, validationLayers) };
            ADH_THROW(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) == VK_SUCCESS,
                      "Failed to create VkInstance!");
        }

        void Instance::Destroy() noexcept {
            Clear();
        }

        Instance::operator VkInstance() noexcept {
            return m_Instance;
        }

        Instance::operator const VkInstance() const noexcept {
            return m_Instance;
        }

        void Instance::MoveConstruct(Instance&& rhs) noexcept {
            m_Instance     = rhs.m_Instance;
            rhs.m_Instance = VK_NULL_HANDLE;
        }

        void Instance::Clear() noexcept {
            if (m_Instance != VK_NULL_HANDLE) {
                vkDestroyInstance(m_Instance, nullptr);
                m_Instance = VK_NULL_HANDLE;
            }
        }
    } // namespace vk
} // namespace adh
