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

#include "Context.hpp"
#include "Allocator.hpp"
#include <string>

namespace adh {
    namespace vk {
        Context::Context(const Window& window, const char* name, const char* path) {
            Create(window, name, path);
        }

        Context::~Context() {
            Clear();
        }

        void Context::Create(const Window& window, const char* name, const char* path) {
            m_Contexts.EmplaceBack(this);
            m_Instance.Create(name);
            m_PhysicalDevice.Create(m_Instance);
            m_Surface.Create(m_Instance, window);
            m_DeviceQueues.Create(m_PhysicalDevice, m_Surface);
            m_Device.Create(m_PhysicalDevice, &m_DeviceQueues);

            if (path) {
                std::string p = path;
                int pos       = p.find("Contents");
                m_Path        = p.substr(0, pos);
            }
        }

        void Context::Destroy() noexcept {
            Clear();
        }

        void Context::Clear() noexcept {
            Allocator::Destroy();
            m_Surface.Destroy();
            m_Device.Destroy();
            m_Instance.Destroy();
        }

        Context* Context::Get() noexcept {
            return m_Contexts[0]; // TODO: Multiple contexts
        }

        VkInstance Context::GetInstance() noexcept {
            return m_Instance;
        }

        const VkInstance Context::GetInstance() const noexcept {
            return m_Instance;
        }

        VkPhysicalDevice Context::GetPhysicalDevice() noexcept {
            return m_PhysicalDevice;
        }

        const VkPhysicalDevice Context::GetPhysicalDevice() const noexcept {
            return m_PhysicalDevice;
        }

        VkSurfaceKHR Context::GetSurface() noexcept {
            return m_Surface;
        }

        const VkSurfaceKHR Context::GetSurface() const noexcept {
            return m_Surface;
        }

        DeviceQueueData Context::GetQueue(DeviceQueues::Family index) noexcept {
            return m_DeviceQueues[index];
        }

        const DeviceQueueData Context::GetQueue(DeviceQueues::Family index) const noexcept {
            return m_DeviceQueues[index];
        }

        VkDevice Context::GetDevice() noexcept {
            return m_Device;
        }

        const VkDevice Context::GetDevice() const noexcept {
            return m_Device;
        }

        const std::string Context::GetDataDirectory() const noexcept {
#if defined(ADH_WINDOWS)
            return DATA_DIRECTORY;
#else
#    if defined(ADH_INSTALL)
            return DATA_DIRECTORY;
#    else
            return m_Path + DATA_DIRECTORY;
#    endif
#endif
        }
    } // namespace vk
} // namespace adh
