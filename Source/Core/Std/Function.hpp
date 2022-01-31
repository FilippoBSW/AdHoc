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
#include "Utility.hpp"
#include <Utility.hpp>

#include <iostream>

namespace adh {
    template <typename T>
    class Function;

    template <typename R, typename... Args>
    class Function<R(Args...)> {
      private:
        using ReturnType        = R;
        using InvokeFuncType    = R (*)(void*, Args&&...);
        using ConstructFuncType = void (*)(void*, void*);
        using DestructFuncType  = void (*)(void*);

      public:
        Function() : m_Data{},
                     m_Size{},
                     m_Invoke{},
                     m_Construct{},
                     m_Destroy{} {
        }

        template <typename Functor>
        Function(Functor func) {
            InitializeConstruct(Move(func));
        }

        Function(const Function& rhs) {
            CopyConstruct(rhs);
        }

        Function& operator=(const Function& rhs) {
            Clear();
            CopyConstruct(rhs);

            return *this;
        }

        Function(Function&& rhs) noexcept {
            MoveConstruct(Move(rhs));
        }

        Function& operator=(Function&& rhs) noexcept {
            Clear();
            MoveConstruct(Move(rhs));

            return *this;
        }

        ~Function() {
            Clear();
        }

        ReturnType operator()(Args&&... args) noexcept {
            return m_Invoke(m_Data, Forward<Args>(args)...);
        }

        operator bool() const noexcept {
            return m_Data;
        }

        template <typename Functor>
        void operator=(Functor func) {
            Clear();
            InitializeConstruct(Move(func));
        }

        void operator=(nullptr_t) noexcept {
            Clear();
        }

      private:
        template <typename Functor>
        void InitializeConstruct(Functor func) {
            m_Data      = new char[sizeof(Functor)];
            m_Size      = sizeof(Functor);
            m_Invoke    = reinterpret_cast<InvokeFuncType>(Invoke<Functor>);
            m_Construct = reinterpret_cast<ConstructFuncType>(Construct<Functor>);
            m_Destroy   = reinterpret_cast<DestructFuncType>(Destroy<Functor>);

            m_Construct(m_Data, reinterpret_cast<char*>(&func));
        }

        void CopyConstruct(const Function& rhs) {
            m_Size      = rhs.m_Size;
            m_Invoke    = rhs.m_Invoke;
            m_Construct = rhs.m_Construct;
            m_Destroy   = rhs.m_Destroy;

            m_Data = new char[m_Size];
            m_Construct(m_Data, rhs.m_Data);
        }

        void MoveConstruct(Function&& rhs) noexcept {
            m_Size      = rhs.m_Size;
            m_Invoke    = rhs.m_Invoke;
            m_Construct = rhs.m_Construct;
            m_Destroy   = rhs.m_Destroy;
            m_Data      = rhs.m_Data;

            rhs.m_Data = nullptr;
        }

        void Clear() noexcept {
            if (m_Data) {
                m_Destroy(m_Data);
                delete[] m_Data;
                m_Data = nullptr;
            }
        }

      private:
        template <typename Functor>
        static ReturnType Invoke(Functor* func, Args&&... args) noexcept {
            return (*func)(Forward<Args>(args)...);
        }

        template <typename Functor>
        static void Construct(Functor* lhs, Functor* rhs) {
            new (lhs) Functor(*rhs);
        }

        template <typename Functor>
        static void Destroy(Functor* func) noexcept {
            func->~Functor();
        }

      private:
        char* m_Data;
        std::size_t m_Size;
        InvokeFuncType m_Invoke;
        ConstructFuncType m_Construct;
        DestructFuncType m_Destroy;
    };
} // namespace adh
