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

#include "Shader.hpp"
#include "Context.hpp"
#include "vulkan/vulkan_core.h"
#include <Std/File.hpp>

namespace adh {
    namespace vk {
        Shader::Shader() noexcept {
        }

        Shader::Shader(Shader&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        Shader& Shader::operator=(Shader&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));
            return *this;
        }

        Shader::~Shader() {
            Clear();
        }

        VkPipelineShaderStageCreateInfo& Shader::Get() noexcept {
            return m_ShaderStageCreateInfo[0];
        }

        const VkPipelineShaderStageCreateInfo& Shader::Get() const noexcept {
            return m_ShaderStageCreateInfo[0];
        }

        std::size_t Shader::GetSize() const noexcept {
            return m_ShaderStageCreateInfo.GetSize();
        }

        void Shader::Destroy() noexcept {
            Clear();
        }

        VkShaderModule Shader::LoadFile(const std::string& shader) {
            ADH_THROW(!shader.empty(), "Null shader file path!");
            auto context{ Context::Get() };
            std::string path{ Context::Get()->GetDataDirectory() + "Resources/Shaders/" + shader + ".spv" };
            File file(path.data(), File::Mode::ReadBinary);
            auto size{ file.GetSize() };
            char* buffer{ static_cast<char*>(ADH_STACK_ALLOC(size)) };
            std::memset(buffer, 0, size);
            if (buffer) {
                file.Read(buffer, size);
            }
            auto info{ initializers::ShaderModuleCreateInfo(size, buffer) };
            VkShaderModule returnValue;
            ADH_THROW(vkCreateShaderModule(context->GetDevice(), &info, nullptr, &returnValue) == VK_SUCCESS,
                      "Failed to create shader module!");
            return returnValue;
        }

        void Shader::MoveConstruct(Shader&& rhs) noexcept {
            m_ShaderModules         = Move(rhs.m_ShaderModules);
            m_ShaderStageCreateInfo = Move(rhs.m_ShaderStageCreateInfo);
            m_SpecializationEntry   = rhs.m_SpecializationEntry;
            m_SpecializationInfo    = rhs.m_SpecializationInfo;
            m_EnablePCF             = rhs.m_EnablePCF;
        }

        void Shader::Clear() noexcept {
            auto device{ Context::Get()->GetDevice() };
            vkDeviceWaitIdle(device);
            for (std::size_t i{}; i != m_ShaderModules.GetSize(); ++i) {
                vkDestroyShaderModule(device, m_ShaderModules[i], nullptr);
                m_ShaderModules[i] = VK_NULL_HANDLE;
            }
        }
    } // namespace vk
} // namespace adh
