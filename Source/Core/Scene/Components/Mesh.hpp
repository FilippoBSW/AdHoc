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
#include <Std/Array.hpp>
#include <Std/SharedPtr.hpp>
#include <Vulkan/IndexBuffer.hpp>
#include <Vulkan/VertexBuffer.hpp>
#include <string>

#include <Vertex.hpp>
#include <unordered_map>

namespace adh {

    struct MeshBufferData {
        vk::IndexBuffer index;
        vk::VertexBuffer vertex;
        Array<Vertex> vertices;
        Array<std::uint32_t> indices;
        std::string meshName;
        std::string meshFilePath;
    };

    class Mesh {
      public:
        void Bind(VkCommandBuffer commandBuffer) noexcept {
            bufferData->index.Bind(commandBuffer);
            bufferData->vertex.Bind(commandBuffer);
        }

        std::uint32_t GetIndexCount() const noexcept {
            return bufferData->index.GetCount();
        }

        void Load(const std::string& meshPath);

        void Load2(const char* fileName);

      public:
        SharedPtr<MeshBufferData> bufferData;
        bool toDraw{ true };
        inline static std::unordered_map<std::string, SharedPtr<MeshBufferData>> meshes;
    };
} // namespace adh
