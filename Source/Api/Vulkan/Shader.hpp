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
#include "Initializers.hpp"
#include <Std/Array.hpp>
#include <Utility.hpp>

#include <string>
#include <vulkan/vulkan.h>

// TODO: Add --target-env vulkan1.2 to shaders for ray tracing
namespace adh {
    namespace vk {
        class Shader {
          public:
            Shader() noexcept;

            template <typename... Args>
            inline Shader(Args&&... args);

            Shader(const Shader&) = delete;

            Shader& operator=(const Shader&) = delete;

            Shader(Shader&& rhs) noexcept;

            Shader& operator=(Shader&& rhs) noexcept;

            ~Shader();

            template <typename Head, typename... Args>
            inline void Create(Head&& head, Args&&... args);

            VkPipelineShaderStageCreateInfo& Get() noexcept;

            const VkPipelineShaderStageCreateInfo& Get() const noexcept;

            std::size_t GetSize() const noexcept;

            void Destroy() noexcept;

          private:
            [[nodiscard]] VkShaderModule LoadFile(const std::string& shader);

            void MoveConstruct(Shader&& rhs) noexcept;

            void Clear() noexcept;

          private:
            Array<VkShaderModule> m_ShaderModules;
            Array<VkPipelineShaderStageCreateInfo> m_ShaderStageCreateInfo;
            VkSpecializationMapEntry m_SpecializationEntry{};
            VkSpecializationInfo m_SpecializationInfo{};
            std::uint32_t m_EnablePCF{ 1u };
        };
    } // namespace vk
} // namespace adh

#include "Shader.inl"
