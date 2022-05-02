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
#include <iostream>

#if defined(ADH_APPLE)
namespace adh {
    Audio::Audio(const char* filePath) {
        Create(filePath);
    }

    Audio::~Audio() = default;

    void Audio::Create(const char* filePath) {
        @autoreleasepool {
            NSString* s = [NSString stringWithUTF8String:filePath];
            mSound      = [[NSSound alloc] initWithContentsOfFile:s
                                                 byReference:NO];
        }
    }

    void Audio::Play() {
        [mSound play];
    }

    void Audio::Stop() {
        [mSound stop];
    }

    void Audio::Pause() {
        [mSound pause];
    }

    void Audio::Resume() {
        [mSound resume];
    }

    void Audio::Loop(bool loop) {
        mSound.loops = loop;
    }

    bool Audio::IsPlaying() const noexcept {
        return mSound.playing;
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

    void Audio::Create(const char* filePath) {
        mWaveOpen = { 0, 0, reinterpret_cast<LPCSTR>(MCI_DEVTYPE_WAVEFORM_AUDIO), filePath, 0, 0 }, mStatus{ 0, 0, MCI_STATUS_MODE, 0 };
        mciSendCommandA(NULL, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT | MCI_OPEN_TYPE_ID, reinterpret_cast<DWORD_PTR>(&mWaveOpen));
    }

    void Audio::Play() {
        mciSendCommand(mWaveOpen.wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, NULL);
        mciSendCommand(mWaveOpen.wDeviceID, MCI_PLAY, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT | MCI_OPEN_TYPE_ID, NULL);
    }

    void Audio::Stop() {
        mciSendCommand(mWaveOpen.wDeviceID, MCI_STOP, MCI_WAIT, NULL);
    }

    void Audio::Pause() {
    }

    void Audio::Resume() {
    }

    void Audio::Loop(bool loop) {
    }

    bool Audio::IsPlaying() const noexcept {
        return false;
    }
} // namespace adh
#endif