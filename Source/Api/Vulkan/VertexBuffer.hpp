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
#include "Buffer.hpp"
#include <Std/Array.hpp>

namespace adh {
    namespace vk {
        class VertexBuffer {
          public:
            VertexBuffer() noexcept = default;

            template <typename T>
            inline VertexBuffer(const Array<T>& vertices);

            VertexBuffer(const void* data, std::size_t size);

            VertexBuffer(const VertexBuffer&& rhs) = delete;

            VertexBuffer& operator=(const VertexBuffer&& rhs) = delete;

            VertexBuffer(VertexBuffer&& rhs) noexcept;

            VertexBuffer& operator=(VertexBuffer&& rhs) noexcept;

            ~VertexBuffer();

            template <typename T>
            inline void Create(const Array<T>& vertices);

            void Create(const void* data, std::size_t size);

            void Bind(VkCommandBuffer commandBuffer) noexcept;

            void Destroy() noexcept;

            operator bool() const noexcept;

          private:
            void MoveConstruct(VertexBuffer&& rhs) noexcept;

            void Clear() noexcept;
            ;

          private:
            Buffer m_Buffer;
        };
    } // namespace vk
} // namespace adh

namespace adh {
    namespace vk {
        template <typename T>
        VertexBuffer::VertexBuffer(const Array<T>& vertices) {
            Create(vertices.GetData(), sizeof(typename Array<T>::Type) * vertices.GetSize());
        }

        template <typename T>
        void VertexBuffer::Create(const Array<T>& vertices) {
            Create(vertices.GetData(), sizeof(typename Array<T>::Type) * vertices.GetSize());
        }
    } // namespace vk
} // namespace adh
