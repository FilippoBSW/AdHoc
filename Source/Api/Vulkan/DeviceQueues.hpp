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

#include <optional>
#include <set>
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        struct DeviceQueueData {
            VkQueue queue;
            std::optional<std::uint32_t> index;
        };

        class DeviceQueues {
          public:
            enum Family {
                eGraphics,
                eCompute,
                eTransfer,
                eSparse,
                ePrensent
            };

          public:
            DeviceQueues() noexcept;

            DeviceQueues(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

            void Create(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

            std::set<std::uint32_t> GetUniqueQueuesIDs() const;

            DeviceQueueData operator[](Family index) const noexcept;

          public:
            union {
                struct {
                    DeviceQueueData graphics;
                    DeviceQueueData compute;
                    DeviceQueueData transfer;
                    DeviceQueueData sparse;
                    DeviceQueueData present;
                };
                DeviceQueueData queue[5];
            };
        };
    } // namespace vk
} // namespace adh
