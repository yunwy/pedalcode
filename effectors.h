#pragma once
#include <iostream>
#include <tuple> // tuple, apply
#include <algorithm> // clamp
#include <cmath>
#include <vector>


struct Clean {
    static constexpr const char* name = "Clean";
    float a;

    void process(float* signal, int n) {
    }

    void nameprint() const {
        std::cout << "  " << name << std::endl; 
    }
};


struct Distortion {
    static constexpr const char* name = "Distortion";
    float gain;
    float limit; // limit < 1

    void process(float* signal, int n) {
        for (int i = 0; i < n; i++) {
            signal[i] = std::clamp(signal[i]*gain, -limit, limit);
        }
    }

    void nameprint() const {
        std::cout << "  " << name << " (gain=" << gain << ", limit=" << limit << ")" << std::endl; 
    }
};


struct Overdrive {
    static constexpr const char* name = "Overdrive";
    float gain;

    void process(float* signal, int n) {
        for (int i = 0; i < n; i++) {
            signal[i] = std::tanh(signal[i]*gain);
        }
    }

    void nameprint() const {
        std::cout << "  " << name << " (gain=" << gain << ")" << std::endl; 
    }
};


struct Bitcrusher {
    static constexpr const char* name = "Bitcrusher";
    int bits; // < 16
    int downsample;
    int counter = 0;
    float hold = 0.0f;

    void process(float* signal, int n) {
        const float step = 2.0f / (1 << bits);
        //const float step = std::pow(2.0F, 1-bits);

        for (int i = 0; i < n; i++) {
            if (counter == 0) { // Downsampling
                // Bit crushing
                hold = std::round(signal[i]/step)*step;
                // hold is updated only when counter == 0
            }
            signal[i] = hold;
            counter = (counter + 1)%downsample;
        }
    }

    void nameprint() const {
        std::cout << "  " << name << " (bits=" << bits << ", downsamples=" << downsample << ")" << std::endl; 
    }
};


struct Delay {
    static constexpr const char* name = "Delay";
    static constexpr const int sampleRate = 48000;
    float g; // Feedback. g < 0.8
    float delayTime; // Delay time, s
    float mix; // mix=0: only original, mix=1: only echo
    int D;
    std::vector<float> delayBuffer; // Circular buffer
    int bufferSize; // size of delayBuffer
    int writeIdx = 0;


    Delay(float g_, float delayTime_, float mix_, float maxDelayTime = 2.0f)
        // List of arguments, only in the generator
        // Order has to be same with the above
        : g(std::clamp(g_, 0.0f, 0.9f)),
          delayTime(delayTime_),
          mix(std::clamp(mix_, 0.0f, 1.0f)),
          D(static_cast<int>(delayTime_*sampleRate)),
          delayBuffer(static_cast<size_t>(maxDelayTime*sampleRate) + 1, 0.0f),
          bufferSize(static_cast<int>(delayBuffer.size()))
          {
            D = std::clamp<int>(D, 0, bufferSize - 1);
          }

    void process(float* signal, int n) {
        for (int i = 0; i < n; i++) {
            //int readIdx = (writeIdx - D + bufferSize) % bufferSize;
            int readIdx = writeIdx - D;
            if (readIdx < 0) readIdx += bufferSize;

            float delayed = delayBuffer[readIdx];
            //delayBuffer[writeIdx] = signal[i]; // Without feedback
            delayBuffer[writeIdx] = signal[i] + g*delayed;
            signal[i] = (1.0f - mix)*signal[i] + mix*delayed;
            /*
            1 - mix: dry
            mix: wet
            */

            //writeIdx = (writeIdx + 1) % bufferSize;
            if (++writeIdx >= bufferSize) writeIdx = 0;  
        }
    }

    void nameprint() const {
        std::cout << "  " << name << " (feedback=" << g << ", delay time=" << delayTime << ", mix=" << mix <<")" << std::endl; 
    }
};


template <typename... PEDAL>
struct SequentialBoard {
    std::tuple<PEDAL...> pedals;
    SequentialBoard(PEDAL... pedal) : pedals(pedal...) {}

    void process(float* signal, int n) {
        std::apply([&](auto&... pedal) {
            (pedal.process(signal, n), ...);
        }, pedals);
    }

    void nameprint() const {
        std::cout << "========== Pedal Board ==========" << std::endl;
        std::apply([](const auto&... pedal) {
            (pedal.nameprint(), ...);
        }, pedals);
        std::cout << "=================================" << std::endl;
    }
};
