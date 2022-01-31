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

#include "DeviceQueues.hpp"
#include "Tools.hpp"

namespace adh {
    namespace vk {
        DeviceQueues::DeviceQueues() noexcept : queue{} {
        }

        DeviceQueues::DeviceQueues(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) : queue{} {
            Create(physicalDevice, surface);
        }

        void DeviceQueues::Create(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
            auto queueFamilies{ tools::GetPhysicalDeviceQueueFamilyProperties(physicalDevice) };

            for (std::uint32_t i{}; i != queueFamilies.GetSize(); ++i) {
                if (!graphics.index &&
                    queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    graphics.index = static_cast<std::int32_t>(i);
                }

                if (!compute.index &&
                    queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
                    compute.index = static_cast<std::int32_t>(i);
                }

                if (!transfer.index &&
                    queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
                    transfer.index = static_cast<std::int32_t>(i);
                }

                if (!sparse.index &&
                    queueFamilies[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
                    sparse.index = static_cast<std::int32_t>(i);
                }

                VkBool32 presentSupport{};
                ADH_THROW(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport) == VK_SUCCESS,
                          "Failed to get physical device surface support!");

                if (!present.index && presentSupport) {
                    present.index = static_cast<std::int32_t>(i);
                }
            }
        }

        std::set<std::uint32_t> DeviceQueues::GetUniqueQueuesIDs() const {
            std::set<std::uint32_t> uniqueFamilies;
            if (graphics.index.has_value()) {
                uniqueFamilies.emplace(graphics.index.value());
            }
            if (compute.index.has_value()) {
                uniqueFamilies.emplace(compute.index.value());
            }
            if (transfer.index.has_value()) {
                uniqueFamilies.emplace(transfer.index.value());
            }
            if (sparse.index.has_value()) {
                uniqueFamilies.emplace(sparse.index.value());
            }
            if (present.index.has_value()) {
                uniqueFamilies.emplace(present.index.value());
            }
            return uniqueFamilies;
        }

        DeviceQueueData DeviceQueues::operator[](Family index) const noexcept {
            return queue[index];
        }
    } // namespace vk
} // namespace adh
