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

#include "Device.hpp"
#include "Initializers.hpp"
#include "Tools.hpp"
#include <Std/Utility.hpp>
#include <set>

namespace adh {
    namespace vk {

        Device::Device() noexcept : m_Device{ VK_NULL_HANDLE } {
        }

        Device::Device(VkPhysicalDevice physicalDevice, DeviceQueues* queues) {
            Create(physicalDevice, queues);
        }

        Device::Device(Device&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        Device& Device::operator=(Device&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));
            return *this;
        }

        VkBool32 Device::IsRaytracingSupported() const noexcept {
            return m_SupportsRayTracing;
        }

        Device::~Device() {
            Clear();
        }

        void Device::Create(VkPhysicalDevice physicalDevice, DeviceQueues* queues) {
            auto uniqueQueues{ queues->GetUniqueQueuesIDs() };
            Array<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
            for (std::uint32_t queueFamilyIndex : uniqueQueues) {
                auto queueCreateInfo{ initializers::DeviceQueueCreateInfo(queueFamilyIndex) };
                deviceQueueCreateInfos.EmplaceBack(queueCreateInfo);
            }

            auto physicalDeviceFeatures{ tools::GetPhysicalDeviceFeatures(physicalDevice) };
            auto physicalDeviceProperties{ tools::GetPhysicalDeviceProperties(physicalDevice) };
            Array<const char*> deviceExtentions;
            deviceExtentions.EmplaceBack("VK_KHR_swapchain");
#if !defined(ADH_APPLE)
            // deviceExtentions.EmplaceBack("VK_KHR_shader_non_semantic_info");
#elif defined(ADH_APPLE)
            deviceExtentions.EmplaceBack("VK_KHR_portability_subset");
#endif
            m_SupportsRayTracing = tools::CheckForExtentionSupport(physicalDevice, "VK_KHR_ray_tracing_pipeline");
            if (m_SupportsRayTracing) {
                deviceExtentions.EmplaceBack("VK_KHR_ray_tracing_pipeline");
                deviceExtentions.EmplaceBack("VK_KHR_acceleration_structure");
                deviceExtentions.EmplaceBack("VK_KHR_spirv_1_4");
                deviceExtentions.EmplaceBack("VK_KHR_shader_float_controls");
                deviceExtentions.EmplaceBack("VK_KHR_get_memory_requirements2");
                deviceExtentions.EmplaceBack("VK_EXT_descriptor_indexing");
                deviceExtentions.EmplaceBack("VK_KHR_buffer_device_address");
                deviceExtentions.EmplaceBack("VK_KHR_deferred_host_operations");
                deviceExtentions.EmplaceBack("VK_KHR_pipeline_library");
                deviceExtentions.EmplaceBack("VK_KHR_maintenance3");
                deviceExtentions.EmplaceBack("VK_KHR_maintenance1");
            }
            tools::CheckDeviceExtensionAvailability(physicalDevice, deviceExtentions);

            auto createInfo{ initializers::DeviceCreateInfo(deviceExtentions) };
            createInfo.queueCreateInfoCount = static_cast<std::uint32_t>(deviceQueueCreateInfos.GetSize());
            createInfo.pQueueCreateInfos    = deviceQueueCreateInfos.GetData();
            createInfo.pEnabledFeatures     = &physicalDeviceFeatures;

            if (m_SupportsRayTracing) {
                auto bufferDeviceAddressFeatures{ initializers::PhysicalDeviceBufferDeviceAddressFeatures() };
                auto rayTracingPipelineFeatures{ initializers::PhysicalDeviceRayTracingPipelineFeatures(bufferDeviceAddressFeatures) };
                auto accelerationStructureFeatures{ initializers::PhysicalDeviceAccelerationStructureFeatures(rayTracingPipelineFeatures) };
                createInfo.pNext = &accelerationStructureFeatures;
            }

            ADH_THROW(vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_Device) == VK_SUCCESS,
                      "Failed to create device!");
            GetDeviceQueues(queues);
        }

        void Device::Destroy() noexcept {
            Clear();
        }

        Device::operator VkDevice() noexcept {
            return m_Device;
        }

        Device::operator const VkDevice() const noexcept {
            return m_Device;
        }

        void Device::GetDeviceQueues(DeviceQueues* queues) noexcept {
            if (queues->graphics.index) {
                vkGetDeviceQueue(m_Device, queues->graphics.index.value(), 0u, &queues->graphics.queue);
            }
            if (queues->compute.index) {
                vkGetDeviceQueue(m_Device, queues->compute.index.value(), 0u, &queues->compute.queue);
            }
            if (queues->transfer.index) {
                vkGetDeviceQueue(m_Device, queues->transfer.index.value(), 0u, &queues->transfer.queue);
            }
            if (queues->sparse.index) {
                vkGetDeviceQueue(m_Device, queues->sparse.index.value(), 0u, &queues->sparse.queue);
            }
            if (queues->present.index) {
                vkGetDeviceQueue(m_Device, queues->present.index.value(), 0u, &queues->present.queue);
            }
        }

        void Device::MoveConstruct(Device&& rhs) noexcept {
            m_Device     = rhs.m_Device;
            rhs.m_Device = VK_NULL_HANDLE;
        }

        void Device::Clear() noexcept {
            if (m_Device != VK_NULL_HANDLE) {
                vkDestroyDevice(m_Device, nullptr);
                m_Device = VK_NULL_HANDLE;
            }
        }
    } // namespace vk
} // namespace adh
