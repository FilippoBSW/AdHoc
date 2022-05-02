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

#if defined(ADH_APPLE)
#    include <AppKit/AppKit.h>
#    include <Cocoa/Cocoa.h>
#elif defined(ADH_WINDOWS)

#else
#    error Not supported platform!
#endif

namespace adh {
    class Audio {
      public:
        Audio() = default;

        ~Audio();

        Audio(const char* filePath);

        void Create(const char* filePath);

        void Play();

        void Stop();

        void Pause();

        void Resume();

        void Loop(bool loop);

        bool IsPlaying() const noexcept;

      private:
#if defined(ADH_APPLE)
        NSSound* mSound;
#elif defined(ADH_WINDOWS)

#endif
    };
} // namespace adh