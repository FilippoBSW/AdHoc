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
#include <Std/Stopwatch.hpp>

#include <iostream>

#include <atomic>
#include <future>

#include <Utility.hpp>

// #if defined(ADH_APPLE)
// #    include <AppKit/AppKit.h>
// #    include <Cocoa/Cocoa.h>
// #elif defined(ADH_WINDOWS)
// #    include <windows.h>

// #    include <digitalv.h>
// #    include <mmsystem.h>
// #else
// // TODO:
// #endif

#include <openal-soft/include/AL/al.h>
#include <openal-soft/include/AL/alc.h>

namespace adh {
    class AudioDevice {
      public:
        AudioDevice();

        AudioDevice(const AudioDevice&)            = delete;
        AudioDevice& operator=(const AudioDevice&) = delete;

        AudioDevice(AudioDevice&& rhs) noexcept;

        AudioDevice& operator=(AudioDevice&&) noexcept;

        ~AudioDevice();

        void Create() ADH_NOEXCEPT;

        void Destroy() noexcept;

      private:
        ALCdevice* device;
        ALCcontext* context;
    };
} // namespace adh

namespace adh {
    class Audio {
        // enum class Status {
        //     eInvalid,
        //     ePlaying,
        //     eStopped,
        //     ePaused,
        // };

      public:
        Audio() = default;

        ~Audio();

        void Create(const char* filePath);

        void Create2(const char* fileName);

        // void OnUpdate();

        void Play();

        void Stop();

        void Pause();

        void Loop(bool loop);

        bool IsPlaying() const noexcept;

      private:
        // #if defined(ADH_APPLE)
        //         NSSound* mSound;
        // #elif defined(ADH_WINDOWS)
        //         MCI_WAVE_OPEN_PARMS mWaveOpen;
        //         MCI_STATUS_PARMS mMciStatus;
        // #endif
        // std::chrono::steady_clock::time_point mStart{};
        // std::chrono::steady_clock::time_point mPause{};
        // std::chrono::duration<float> mPauseDuration{};
        // std::atomic<Status> mStatus{ Status::eInvalid };
        int mDuration{};

        ALuint mFormat;
        ALuint mSource;
        float mPitch{ 1 };
        float mGain{ 1 };
        float mPosition[3]{};
        float mVelocity[3]{};
        bool mLoop{ false };
        ALuint mBuffer;
    };
} // namespace adh
