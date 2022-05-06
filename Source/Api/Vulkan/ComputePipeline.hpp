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

#pragma once
#include <Utility.hpp>

#include <iostream>
#include <vulkan/vulkan.h>

namespace adh {
    namespace vk {
        class ComputePipeline {
          public:
            ComputePipeline() noexcept;

            ComputePipeline(
                VkPipelineLayout layout,
                VkPipelineShaderStageCreateInfo stage,
                std::uint32_t localSizeX,
                std::uint32_t elementSizeX,
                std::uint32_t localSizeY,
                std::uint32_t elementSizeY,
                std::uint32_t localSizeZ,
                std::uint32_t elementSizeZ) ADH_NOEXCEPT;

            ComputePipeline(const ComputePipeline& rhs) = delete;

            ComputePipeline& operator=(const ComputePipeline& rhs) = delete;

            ComputePipeline(ComputePipeline&& rhs) noexcept;

            ComputePipeline& operator=(ComputePipeline&& rhs) noexcept;

            ~ComputePipeline();

            void Create(
                VkPipelineLayout layout,
                VkPipelineShaderStageCreateInfo stage,
                std::uint32_t localSizeX,
                std::uint32_t elementSizeX,
                std::uint32_t localSizeY,
                std::uint32_t elementSizeY,
                std::uint32_t localSizeZ,
                std::uint32_t elementSizeZ) ADH_NOEXCEPT;

            void Bind(VkCommandBuffer commandBuffer) noexcept;

            void Dispatch(VkCommandBuffer commandBuffer) noexcept;

            void Destroy() noexcept;

            operator VkPipeline() noexcept;

            operator const VkPipeline() const noexcept;

          private:
            void MoveConstruct(ComputePipeline&& rhs) noexcept;

            void Clear() noexcept;

          private:
            VkPipeline m_Pipeline;

            union {
                std::uint32_t m_Groups[3];
                struct {
                    std::uint32_t x;
                    std::uint32_t y;
                    std::uint32_t z;
                };
            };
        };
    } // namespace vk
} // namespace adh
