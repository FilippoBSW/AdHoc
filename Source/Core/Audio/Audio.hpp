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
        // int mDuration{};

        bool mReady{ false };

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
