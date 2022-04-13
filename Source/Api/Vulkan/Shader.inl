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

namespace adh {
    namespace vk {
        template <typename... Args>
        Shader::Shader(Args&&... args) {
            m_SpecializationEntry = initializers::SpecializationMapEntry(sizeof(std::uint32_t));
            m_SpecializationInfo  = initializers::SpecializationInfo(&m_SpecializationEntry, sizeof(std::uint32_t), &m_EnablePCF);
            Create(Forward<Args>(args)...);
        }

        template <typename Head, typename... Args>
        void Shader::Create(Head&& head, Args&&... args) {
            std::string shader{ head };
            auto pos{ shader.find_last_of(".") };
            auto stage{ shader.substr(pos + 1, std::string::npos) };
            VkShaderStageFlagBits stageFlag{};
            if (stage == "vert") {
                stageFlag = VK_SHADER_STAGE_VERTEX_BIT;
            } else if (stage == "frag") {
                stageFlag = VK_SHADER_STAGE_FRAGMENT_BIT;
            } else if (stage == "comp") {
                stageFlag = VK_SHADER_STAGE_COMPUTE_BIT;
            } else if (stage == "frag") {
                stageFlag = VK_SHADER_STAGE_FRAGMENT_BIT;
            } else if (stage == "rgen") {
                stageFlag = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
            } else if (stage == "rchit") {
                stageFlag = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
            } else if (stage == "rmiss") {
                stageFlag = VK_SHADER_STAGE_MISS_BIT_KHR;
            }
            ADH_THROW(stageFlag, "Invalid shader type!");
            m_ShaderStageCreateInfo.EmplaceBack(
                initializers::PipelineShaderStageCreateInfo(stageFlag,
                                                            m_ShaderModules.EmplaceBack(LoadFile(shader)),
                                                            &m_SpecializationInfo));
            if constexpr (sizeof...(Args) > 0) {
                Create(args...);
            }
        }
    } // namespace vk
} // namespace adh
