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
#include "Device.hpp"
#include "DeviceQueues.hpp"
#include "Instance.hpp"
#include "PhysicalDevice.hpp"
#include "Surface.hpp"
#include <Std/Array.hpp>

#include <string>

namespace adh {
    namespace vk {
        class Context {
          public:
            Context() noexcept = default;

            Context(const Window& window, const char* name, const char* path);

            ~Context();

            void Create(const Window& window, const char* name, const char* path);

            void Destroy() noexcept;

            VkInstance GetInstance() noexcept;

            const VkInstance GetInstance() const noexcept;

            VkPhysicalDevice GetPhysicalDevice() noexcept;

            const VkPhysicalDevice GetPhysicalDevice() const noexcept;

            VkSurfaceKHR GetSurface() noexcept;

            const VkSurfaceKHR GetSurface() const noexcept;

            DeviceQueueData GetQueue(DeviceQueues::Family index) noexcept;

            const DeviceQueueData GetQueue(DeviceQueues::Family index) const noexcept;

            VkDevice GetDevice() noexcept;

            const VkDevice GetDevice() const noexcept;

            const std::string GetDataDirectory() const noexcept;

          private:
            void Clear() noexcept;

          public:
            static Context* Get() noexcept;

          private:
            Instance m_Instance;
            PhysicalDevice m_PhysicalDevice;
            Surface m_Surface;
            DeviceQueues m_DeviceQueues;
            Device m_Device;
            std::string m_Path;

          private:
            inline static Array<Context*> m_Contexts;
        };
    } // namespace vk
} // namespace adh
