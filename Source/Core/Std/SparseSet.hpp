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
#include "Array.hpp"
#include "Utility.hpp"
#include <Utility.hpp>

#include <iostream>
#include <limits>

namespace adh {
    template <std::uint64_t S>
    class SparseSetPage {
      public:
        static constexpr std::uint64_t PageMaxSize{ S / sizeof(std::uint64_t) };

      public:
        SparseSetPage() : data{ new std::uint64_t[PageMaxSize] } {
            std::memset(data, -1, sizeof(std::uint64_t) * PageMaxSize);
        }

        SparseSetPage(const SparseSetPage& rhs) = delete;
        SparseSetPage& operator=(const SparseSetPage& rhs) = delete;

        SparseSetPage(SparseSetPage&& rhs) noexcept {
            data     = rhs.data;
            rhs.data = nullptr;
        }

        SparseSetPage& operator=(SparseSetPage&& rhs) noexcept {
            data     = rhs.data;
            rhs.data = nullptr;

            return *this;
        }

        ~SparseSetPage() {
            delete[] data;
        }

        auto& operator[](std::uint64_t index) {
            return data[index];
        }

        const auto& operator[](std::uint64_t index) const {
            return data[index];
        }

      private:
        std::uint64_t* data;
    };

    class BaseSparseSet {
      public:
        virtual ~BaseSparseSet()                                             = default;
        virtual void Remove(const std::uint32_t&)                            = 0;
        virtual constexpr bool Contains(const std::uint32_t&) const noexcept = 0;
    };

    template <typename T, std::uint64_t S>
    class SparseSet : public BaseSparseSet {
      public:
        static constexpr std::uint64_t nPos{ std::numeric_limits<std::uint64_t>::max() };

      public:
        template <typename... Args>
        auto& Add(const std::uint32_t& id, Args&&... args) {
            if (GetPage(id) >= m_Sparse.GetSize()) {
                m_Sparse.Resize(GetPage(id) + 1u);
            }

            if (!Contains(id)) {
                auto& ret{ m_Dense.EmplaceBack(Forward<Args>(args)...) };
                m_DenseIndex.EmplaceBack(id);
                m_Sparse[GetPage(id)][GetOffset(id)] = m_Dense.GetSize() - 1u;

                return ret;
            }
            return m_Dense[m_Dense.GetSize() - 1u];
        }

        void Remove(const std::uint32_t& id) noexcept {
            auto temp{ m_Dense.GetSize() - 1u };

            if (temp != m_Sparse[GetPage(id)][GetOffset(id)]) {
                m_Dense[m_Sparse[GetPage(id)][GetOffset(id)]] = Move(m_Dense[temp]);

                auto t                                               = m_Sparse[GetPage(id)][GetOffset(m_DenseIndex[temp])];
                m_Sparse[GetPage(id)][GetOffset(m_DenseIndex[temp])] = m_Sparse[GetPage(id)][GetOffset(id)];
                m_DenseIndex[m_Sparse[GetPage(id)][GetOffset(id)]]   = m_DenseIndex[temp];
            }

            m_Sparse[GetPage(id)][GetOffset(id)] = nPos;

            m_Dense.PopBack();
            m_DenseIndex.PopBack();
        }

        void Reserve(std::size_t size) {
            m_Dense.Reserve(size);
            m_DenseIndex.Reserve(size);
            m_Sparse.Resize(size / SparseSetPage<S>::PageMaxSize + 1u);
        }

        constexpr bool Contains(const std::uint32_t& id) const noexcept {
            if (!m_Sparse.IsEmpty()) {
                return m_Sparse[GetPage(id)][GetOffset(id)] != nPos;
            }
            return false;
        }

        auto& Get(const std::uint32_t& id) noexcept {
            return m_Dense[m_Sparse[GetPage(id)][GetOffset(id)]];
        }

        const auto& Get(const std::uint32_t& id) const noexcept {
            return m_Dense[m_Sparse[GetPage(id)][GetOffset(id)]];
        }

        auto& operator[](const std::uint32_t& id) noexcept {
            return Get(id);
        }

        const auto& operator[](const std::uint32_t& id) const noexcept {
            return Get(id);
        }

        auto GetData() const noexcept {
            return m_Dense.GetData();
        }

        auto& GetDense() noexcept {
            return m_Dense;
        }

        auto GetDataIndex(const std::uint32_t& id) const noexcept {
            return m_Sparse[GetPage(id)][GetOffset(id)];
        }

        decltype(auto) GetBegin() noexcept {
            return m_Dense.begin();
        }

        decltype(auto) GetBegin() const noexcept {
            return m_Dense.begin();
        }

        decltype(auto) GetEnd() noexcept {
            return m_Dense.end();
        }

        decltype(auto) GetEnd() const noexcept {
            return m_Dense.end();
        }

        auto GetSize() const noexcept {
            return m_Dense.GetSize();
        }

        auto GetCapacity() const noexcept {
            return m_Dense.GetCapacity();
        }

        auto IsEmpty() const noexcept {
            return m_Dense.IsEmpty();
        }

      private:
        constexpr auto GetPage(const std::uint32_t& id) const noexcept {
            return id / SparseSetPage<S>::PageMaxSize;
        }

        constexpr auto GetOffset(const std::uint32_t& id) const noexcept {
            return id % SparseSetPage<S>::PageMaxSize;
        }

      private:
        Array<SparseSetPage<S>> m_Sparse;
        Array<T> m_Dense;
        Array<std::uint32_t> m_DenseIndex;
    };
} // namespace adh
