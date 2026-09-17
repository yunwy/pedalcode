#pragma once
#include <iostream>
#include <tuple> // tuple, apply
#include <algorithm> // clamp
#include <cmath>


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
    float g; // Feedback. g < 0.8
    int D; // Delay

    void process(float* signal, int n) {
    }

    void nameprint() const {
        std::cout << "  " << name << " (feedback=" << g << ", Delay=" << D << ")" << std::endl; 
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
