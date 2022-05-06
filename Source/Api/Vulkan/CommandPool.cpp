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

#include "CommandPool.hpp"
#include "Context.hpp"
#include "Initializers.hpp"

namespace adh {
    namespace vk {
        CommandPool::CommandPool() {
            m_Pools.emplace(std::this_thread::get_id(), this);
        }

        CommandPool::~CommandPool() {
            Clear();
        }

        VkCommandPool CommandPool::Get(VkCommandPoolCreateFlagBits flag, std::uint32_t queueIndex) {
            const auto& pool{ m_CommandPools.emplace(_internal::CommandPoolData{ flag, queueIndex }) };
            if (pool.second) {
                auto createInfo{ initializers::CommandPoolCreateInfo(flag, queueIndex) };
                ADH_THROW(vkCreateCommandPool(Context::Get()->GetDevice(), &createInfo, nullptr, &pool.first->commandPool) == VK_SUCCESS,
                          "Failed to create command pool!");
            }
            return pool.first->commandPool;
        }

        void CommandPool::Reset(VkCommandPoolCreateFlagBits flag, std::uint32_t queueIndex) ADH_NOEXCEPT {
            auto itr{ m_CommandPools.find({ flag, queueIndex }) };
            if (itr->commandPool != VK_NULL_HANDLE) {
                ADH_THROW(vkResetCommandPool(Context::Get()->GetDevice(), itr->commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT) == VK_SUCCESS,
                          "Failed to reset command poo!");
            }
        }

        void CommandPool::Destroy() noexcept {
            Clear();
        }

        CommandPool* CommandPool::Get() noexcept {
            return m_Pools.find(std::this_thread::get_id())->second;
        }

        void CommandPool::Clear() noexcept {
            auto device{ Context::Get()->GetDevice() };
            vkDeviceWaitIdle(device);
            for (auto&& i : m_CommandPools) {
                vkDestroyCommandPool(device, i.commandPool, nullptr);
                i.commandPool = VK_NULL_HANDLE;
            }
        }
    } // namespace vk
} // namespace adh
