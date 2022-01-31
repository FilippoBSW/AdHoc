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
#include <set>
#include <thread>
#include <unordered_map>
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        namespace _internal {
            struct CommandPoolData {
                VkCommandPoolCreateFlagBits flag;
                std::uint32_t queueIndex;
                mutable VkCommandPool commandPool{ VK_NULL_HANDLE };
            };

            constexpr bool operator<(const CommandPoolData& lhs, const CommandPoolData& rhs) {
                return (lhs.flag < rhs.flag) || (lhs.queueIndex < rhs.queueIndex);
            }
        } // namespace _internal
    }     // namespace vk
} // namespace adh

namespace adh {
    namespace vk {
        class CommandPool {
          public:
            CommandPool();

            CommandPool(const CommandPool& rhs) = delete;

            CommandPool& operator=(const CommandPool& rhs) = delete;

            CommandPool(CommandPool&& rhs) = delete;

            CommandPool& operator=(CommandPool&& rhs) = delete;

            ~CommandPool();

            VkCommandPool Get(VkCommandPoolCreateFlagBits flag, std::uint32_t queueIndex);

            void Reset(VkCommandPoolCreateFlagBits flag, std::uint32_t queueIndex) ADH_NOEXCEPT;

            void Destroy() noexcept;

          private:
            void Clear() noexcept;

          public:
            static CommandPool* Get() noexcept;

          private:
            std::set<_internal::CommandPoolData> m_CommandPools;

          private:
            inline static std::unordered_map<std::thread::id, CommandPool*> m_Pools;
        };
    } // namespace vk
} // namespace adh
