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
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
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

#include <fstream>
#include <string>

// check big vs little endian machine
static bool IsBigEndian(void) {
    int a = 1;
    return !((char*)&a)[0];
}

static int ConvertToInt(char* buffer, int len) {
    int a = 0;

    if (!IsBigEndian()) {
        for (int i = 0; i < len; ++i) {
            ((char*)&a)[i] = buffer[i];
        }
    } else {
        for (int i = 0; i < len; ++i) {
            ((char*)&a)[3 - i] = buffer[i];
        }
    }

    return a;
}

// Location and size of data is found here:
// http://www.topherlee.com/software/pcm-tut-wavformat.html
static char* LoadWAV(std::string filename, int& channels, int& sampleRate,
                     int& bps, int& size) {
    char buffer[4];

    std::ifstream in(filename.c_str());
    in.read(buffer, 4);

    if (strncmp(buffer, "RIFF", 4) != 0) {
        std::cout << "Error here, not a valid WAV file, RIFF not found in header\n "
                     "This was found instead: "
                  << buffer[0] << buffer[1] << buffer[2] << buffer[3] << std::endl;
    }

    in.read(buffer, 4); // size of file. Not used. Read it to skip over it.

    in.read(buffer, 4); // Format, should be WAVE

    if (strncmp(buffer, "WAVE", 4) != 0) {
        std::cout << "Error here, not a valid WAV file, WAVE not found in "
                     "header.\n This was found instead: "
                  << buffer[0] << buffer[1] << buffer[2] << buffer[3] << std::endl;
    }

    in.read(buffer, 4); // Format Space Marker. should equal fmt (space)

    if (strncmp(buffer, "fmt ", 4) != 0) {
        std::cout << "Error here, not a valid WAV file, Format Marker not found in "
                     "header.\n This was found instead: "
                  << buffer[0] << buffer[1] << buffer[2] << buffer[3] << std::endl;
    }

    in.read(buffer, 4); // Length of mFormat data. Should be 16 for PCM, meaning uncompressed.

    if (ConvertToInt(buffer, 4) != 16) {
        std::cout << "Error here, not a valid WAV file, mFormat length wrong in "
                     "header.\n This was found instead: "
                  << ConvertToInt(buffer, 4) << std::endl;
    }

    in.read(buffer, 2); // Type of mFormat, 1 = PCM

    if (ConvertToInt(buffer, 2) != 1) {
        std::cout << "Error here, not a valid WAV file, file not in PCM mFormat.\n "
                     "This was found instead: "
                  << ConvertToInt(buffer, 4) << std::endl;
    }

    in.read(buffer, 2); // Get number of channels.

    // Assume at this point that we are dealing with a WAV file. This value is
    // needed by OpenAL
    channels = ConvertToInt(buffer, 2);

    in.read(buffer, 4); // Get sampler rate.

    sampleRate = ConvertToInt(buffer, 4);

    // Skip Byte Rate and Block Align. Maybe use later?
    in.read(buffer, 4); // Block Align
    in.read(buffer, 2); // ByteRate

    in.read(buffer, 2); // Get Bits Per Sample

    bps = ConvertToInt(buffer, 2);

    // Skip character data, which marks the start of the data that we care about.
    in.read(buffer, 4); //"data" chunk.

    in.read(buffer, 4); // Get size of the data

    size = ConvertToInt(buffer, 4);

    if (size < 0) {
        std::cout << "Error here, not a valid WAV file, size of file reports 0.\n "
                     "This was found instead: "
                  << size << std::endl;
    }

    char* data = new char[size];

    in.read(data, size); // Read audio data into (mBuffer, return.

    in.close();

    return data;
}

namespace adh {
    AudioDevice::AudioDevice() : device{}, context{} {}

    AudioDevice::AudioDevice(AudioDevice&& rhs) noexcept {
        device      = rhs.device;
        context     = rhs.context;
        rhs.device  = nullptr;
        rhs.context = nullptr;
    }

    AudioDevice& AudioDevice::operator=(AudioDevice&& rhs) noexcept {
        Destroy();
        device      = rhs.device;
        context     = rhs.context;
        rhs.device  = nullptr;
        rhs.context = nullptr;
        return *this;
    }

    AudioDevice::~AudioDevice() { Destroy(); }

    void AudioDevice::Create() ADH_NOEXCEPT {
        device = alcOpenDevice(nullptr);
        ADH_THROW(device, "Failed to open alc device!");
        context = alcCreateContext(device, nullptr);
        ADH_THROW(context, "Failed to create alc context!");
        alcMakeContextCurrent(context);
    }

    void AudioDevice::Destroy() noexcept {
        if (context) {
            alcDestroyContext(context);
        }
        if (device) {
            alcCloseDevice(device);
        }
    }
} // namespace adh


namespace adh {
    Audio::~Audio() {
        alDeleteSources(1, &mSource);
        alDeleteBuffers(1, &mBuffer);
    }

    void Audio::Create(const char* filePath) {
        // Generate buffer
        alGenBuffers(1, &mBuffer);
        int channel, sampleRate, bps, size;
        char* data = LoadWAV(filePath, channel, sampleRate, bps, size);

        if (channel == 1) {
            if (bps == 8) {
                mFormat = AL_FORMAT_MONO8;
            } else {
                mFormat = AL_FORMAT_MONO16;
            }
        } else {
            if (bps == 8) {
                mFormat = AL_FORMAT_STEREO8;
            } else {
                mFormat = AL_FORMAT_STEREO16;
            }
        }
        alBufferData(mBuffer, mFormat, data, size, sampleRate);

        // Generate source
        alGenSources(1, &mSource);
        alSourcef(mSource, AL_PITCH, mPitch);
        alSourcef(mSource, AL_GAIN, mGain);
        alSource3f(mSource, AL_POSITION, mPosition[0], mPosition[1], mPosition[2]);
        alSource3f(mSource, AL_VELOCITY, mVelocity[0], mVelocity[1], mVelocity[2]);
        alSourcei(mSource, AL_LOOPING, mLoop);
        alSourcei(mSource, AL_BUFFER, mBuffer);

        delete[] data;
    }

    void Audio::Create2(const char* fileName) {
        Create((vk::Context::Get()->GetDataDirectory() + "Assets/Audio/" + fileName).data());
    }

    // void Audio::OnUpdate() {
    //     if (mDuration && IsPlaying()) {
    //         if (std::chrono::duration<float>{ std::chrono::steady_clock::now() -mStart }.count() > mDuration + mPauseDuration.count()) {
    //             Stop();
    //         }
    //     }
    // }

    void Audio::Play() {
        alSourcePlay(mSource);
    }

    void Audio::Stop() {
        alSourceStop(mSource);
    }

    void Audio::Pause() {
        alSourcePause(mSource);
    }

    void Audio::Loop(bool loop) {
        mLoop = loop;
        alSourcei(mSource, AL_LOOPING, mLoop);
    }

    bool Audio::IsPlaying() const noexcept {
        ALint state;
        alGetSourcei(mSource, AL_SOURCE_STATE, &state);

        if (state == AL_PLAYING) {
            return true;
        } else {
            return false;
        }
    }

} // namespace adh

// #include "Audio.hpp"
// #include <Vulkan/Context.hpp>
// #include <iostream>

// #if defined(ADH_APPLE)
// namespace adh {
//     Audio::Audio(const char* filePath) {
//         Create(filePath);
//     }

//     Audio::~Audio() {
//         [mSound stop];
//     }

//     void Audio::Create(const char* filePath, float seconds) {
//         @autoreleasepool {
//             NSString* s = [NSString stringWithUTF8String:filePath];
//             mSound      = [[NSSound alloc] initWithContentsOfFile:s
//                                                  byReference:NO];
//         }

//         mDuration = seconds;
//     }

//     void Audio::Create2(const char* fileName, float seconds) {
//         Create((vk::Context::Get()->GetDataDirectory() + "Assets/Audio/" + fileName).data(), seconds);
//     }

//     void Audio::OnUpdate() {
//         if (mDuration && IsPlaying()) {
//             if (std::chrono::duration<float>{ std::chrono::steady_clock::now() - mStart }.count() > mDuration + mPauseDuration.count()) {
//                 Stop();
//             }
//         }
//         if (mLoop) {
//             if (!IsPlaying()) {
//                 Play();
//             }
//         }
//     }

//     void Audio::Play() {
//         if (!IsPlaying()) {
//             [mSound play];
//         } else {
//             [mSound stop];
//             [mSound play];
//         }
//         mStart  = std::chrono::steady_clock::now();
//         mStatus = Status::ePlaying;
//     }

//     void Audio::Stop() {
//         [mSound stop];
//         mStatus = Status::eStopped;
//     }

//     void Audio::Pause() {
//         [mSound pause];
//         mPause  = std::chrono::steady_clock::now();
//         mStatus = Status::ePaused;
//     }

//     void Audio::Resume() {
//         [mSound resume];
//         mPauseDuration = { std::chrono::steady_clock::now() - mPause };
//         mStatus        = Status::ePlaying;
//     }

//     void Audio::Loop(bool loop) {
//         mLoop = loop;
//     }

//     bool Audio::IsPlaying() const noexcept {
//         return mSound.playing && mStatus == Status::ePlaying;
//     }
// } // namespace adh

// #elif defined(ADH_WINDOWS)
// #    pragma comment(lib, "winmm.lib")

// namespace adh {
//     Audio::Audio(const char* filePath) {
//         Create(filePath);
//     }

//     Audio::~Audio() {
//         mciSendCommand(mWaveOpen.wDeviceID, MCI_CLOSE, NULL, NULL);
//     }

//     void Audio::Create(const char* filePath, float seconds) {
//         std::int32_t size{ MultiByteToWideChar(CP_UTF8, 0, filePath, -1, nullptr, 0) };
//         auto file = new wchar_t[size];
//         MultiByteToWideChar(CP_UTF8, 0, filePath, -1, file, size);

//         mWaveOpen  = MCI_WAVE_OPEN_PARMS{ 0, 0, reinterpret_cast<LPCWSTR>(MCI_DEVTYPE_WAVEFORM_AUDIO), file, 0, 0 };
//         mMciStatus = MCI_STATUS_PARMS{ 0, 0, MCI_STATUS_MODE, 0 };
//         mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT | MCI_OPEN_TYPE_ID, reinterpret_cast<DWORD_PTR>(&mWaveOpen));

//         delete[] file;

//         mDuration = seconds;
//     }

//     void Audio::Create2(const char* fileName, float seconds) {
//         Create((vk::Context::Get()->GetDataDirectory() + "Assets/Audio/" + fileName).data(), seconds);
//     }

//     void Audio::OnUpdate() {
//         if (mDuration && IsPlaying()) {
//             if (std::chrono::duration<float>{ std::chrono::steady_clock::now() - mStart }.count() > mDuration + mPauseDuration.count()) {
//                 Stop();
//             }
//         }
//         if (mLoop) {
//             if (!IsPlaying()) {
//                 Play();
//             }
//         }
//     }

//     void Audio::Play() {
//         if (!IsPlaying()) {
//             mciSendCommand(mWaveOpen.wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, NULL);
//             mciSendCommand(mWaveOpen.wDeviceID, MCI_PLAY, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT | MCI_OPEN_TYPE_ID, NULL);
//         } else {
//             mciSendCommand(mWaveOpen.wDeviceID, MCI_STOP, MCI_WAIT, NULL);
//             mciSendCommand(mWaveOpen.wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, NULL);
//             mciSendCommand(mWaveOpen.wDeviceID, MCI_PLAY, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT | MCI_OPEN_TYPE_ID, NULL);
//         }
//         mStart  = std::chrono::steady_clock::now();
//         mStatus = Status::ePlaying;
//     }

//     void Audio::Stop() {
//         mciSendCommand(mWaveOpen.wDeviceID, MCI_STOP, MCI_WAIT, NULL);
//         mStatus = Status::eStopped;
//     }

//     void Audio::Pause() {
//         mciSendCommand(mWaveOpen.wDeviceID, MCI_PAUSE, MCI_WAIT, NULL);
//         mPause  = std::chrono::steady_clock::now();
//         mStatus = Status::ePaused;
//     }

//     void Audio::Resume() {
//         mciSendCommand(mWaveOpen.wDeviceID, MCI_RESUME, MCI_WAIT, NULL);
//         mPauseDuration = { std::chrono::steady_clock::now() - mPause };
//         mStatus        = Status::ePlaying;
//     }

//     void Audio::Loop(bool loop) {
//         mLoop = loop;
//     }

//     bool Audio::IsPlaying() const noexcept {
//         mciSendCommand(mWaveOpen.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, reinterpret_cast<DWORD_PTR>(&mMciStatus));
//         if (mMciStatus.dwReturn == MCI_MODE_PLAY && mStatus == Status::ePlaying) {
//             return true;
//         } else {
//             return false;
//         }
//     }
// } // namespace adh
// #elif defined(ADH_LINUX)
// namespace adh {
//     Audio::Audio(const char* filePath) {
//     }

//     Audio::~Audio() {
//     }

//     void Audio::Create(const char* filePath, float seconds) {
//     }

//     void Audio::Create2(const char* fileName, float seconds) {
//     }

//     void Audio::OnUpdate() {
//     }

//     void Audio::Play() {
//     }

//     void Audio::Stop() {
//     }

//     void Audio::Pause() {
//     }

//     void Audio::Resume() {
//     }

//     void Audio::Loop(bool loop) {
//     }

//     bool Audio::IsPlaying() const noexcept {
//         return false;
//     }
// } // namespace adh
// #endif
