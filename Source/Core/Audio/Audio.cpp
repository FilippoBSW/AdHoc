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

#include "Audio.hpp"
#include <Vulkan/Context.hpp>
#include <iostream>

#if defined(ADH_APPLE)
namespace adh {
    Audio::Audio(const char* filePath) {
        Create(filePath);
    }

    Audio::~Audio() = default;

    void Audio::Create(const char* filePath, float seconds) {
        @autoreleasepool {
            NSString* s = [NSString stringWithUTF8String:filePath];
            mSound      = [[NSSound alloc] initWithContentsOfFile:s
                                                 byReference:NO];
        }

        mDuration = seconds;
    }

    void Audio::Create2(const char* fileName, float seconds) {
        Create((vk::Context::Get()->GetDataDirectory() + "Assets/Audio/" + fileName).data(), seconds);
    }

    void Audio::OnUpdate() {
        if (mDuration && IsPlaying()) {
            if (std::chrono::duration<float>{ std::chrono::steady_clock::now() - mStart }.count() > mDuration + mPauseDuration.count()) {
                Stop();
            }
        }
        if (mLoop) {
            if (!IsPlaying()) {
                Play();
            }
        }
    }

    void Audio::Play() {
        [mSound play];
        mStart  = std::chrono::steady_clock::now();
        mStatus = Status::ePlaying;
    }

    void Audio::Stop() {
        [mSound stop];
        mStatus = Status::eStopped;
    }

    void Audio::Pause() {
        [mSound pause];
        mPause  = std::chrono::steady_clock::now();
        mStatus = Status::ePaused;
    }

    void Audio::Resume() {
        [mSound resume];
        mPauseDuration = { std::chrono::steady_clock::now() - mPause };
        mStatus        = Status::ePlaying;
    }

    void Audio::Loop(bool loop) {
        mLoop = loop;
    }

    bool Audio::IsPlaying() const noexcept {
        return mSound.playing && mStatus == Status::ePlaying;
    }
} // namespace adh

#elif defined(ADH_WINDOWS)
#    pragma comment(lib, "winmm.lib")

namespace adh {
    Audio::Audio(const char* filePath) {
        Create(filePath);
    }

    Audio::~Audio() {
        mciSendCommand(mWaveOpen.wDeviceID, MCI_CLOSE, NULL, NULL);
    }

    void Audio::Create(const char* filePath, float seconds) {
        std::int32_t size{ MultiByteToWideChar(CP_UTF8, 0, filePath, -1, nullptr, 0) };
        auto file = new wchar_t[size];
        MultiByteToWideChar(CP_UTF8, 0, filePath, -1, file, size);

        mWaveOpen  = MCI_WAVE_OPEN_PARMS{ 0, 0, reinterpret_cast<LPCWSTR>(MCI_DEVTYPE_WAVEFORM_AUDIO), file, 0, 0 };
        mMciStatus = MCI_STATUS_PARMS{ 0, 0, MCI_STATUS_MODE, 0 };
        mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT | MCI_OPEN_TYPE_ID, reinterpret_cast<DWORD_PTR>(&mWaveOpen));

        delete[] file;

        mDuration = seconds;
    }

    void Audio::Create2(const char* fileName, float seconds) {
        Create((vk::Context::Get()->GetDataDirectory() + "Assets/Audio/" + fileName).data(), seconds);
    }

    void Audio::OnUpdate() {
        if (mDuration && IsPlaying()) {
            if (std::chrono::duration<float>{ std::chrono::steady_clock::now() - mStart }.count() > mDuration + mPauseDuration.count()) {
                Stop();
            }
        }
        if (mLoop) {
            if (!IsPlaying()) {
                Play();
            }
        }
    }

    void Audio::Play() {
        mciSendCommand(mWaveOpen.wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, NULL);
        mciSendCommand(mWaveOpen.wDeviceID, MCI_PLAY, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT | MCI_OPEN_TYPE_ID, NULL);
        mStart  = std::chrono::steady_clock::now();
        mStatus = Status::ePlaying;
    }

    void Audio::Stop() {
        mciSendCommand(mWaveOpen.wDeviceID, MCI_STOP, MCI_WAIT, NULL);
        mStatus = Status::eStopped;
    }

    void Audio::Pause() {
        mciSendCommand(mWaveOpen.wDeviceID, MCI_PAUSE, MCI_WAIT, NULL);
        mPause  = std::chrono::steady_clock::now();
        mStatus = Status::ePaused;
    }

    void Audio::Resume() {
        mciSendCommand(mWaveOpen.wDeviceID, MCI_RESUME, MCI_WAIT, NULL);
        mPauseDuration = { std::chrono::steady_clock::now() - mPause };
        mStatus        = Status::ePlaying;
    }

    void Audio::Loop(bool loop) {
        mLoop = loop;
    }

    bool Audio::IsPlaying() const noexcept {
        mciSendCommand(mWaveOpen.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, reinterpret_cast<DWORD_PTR>(&mMciStatus));
        if (mMciStatus.dwReturn == MCI_MODE_PLAY && mStatus == Status::ePlaying) {
            return true;
        } else {
            return false;
        }
    }
} // namespace adh
#endif
