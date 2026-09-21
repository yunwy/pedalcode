#include <iostream>
#include <algorithm>

// For ASIO
#include <windows.h>
#include <asio.h>
#include <asiosys.h>
#include <asiodrivers.h>

#include "effectors.h"
#include "effectorboard.h"


ASIODriverInfo driverInfo;
ASIOBufferInfo bufferInfos[3]; // input 1, output 2
ASIOCallbacks callbacks;

long bufferSize = 256;
float temp[4096];


void bufferSwitch(long index, ASIOBool processNow) {
    int32_t* in = (int32_t*)bufferInfos[0].buffers[index];
    int32_t* outL = (int32_t*)bufferInfos[1].buffers[index];
    int32_t* outR = (int32_t*)bufferInfos[2].buffers[index];

    for (long i = 0; i < bufferSize; i++){
        temp[i] = in[i]/2147483648.0f;
    }
    board.process(temp, bufferSize);

    for (long i = 0; i < bufferSize; i++){
        float v = std::clamp(temp[i], -1.0f, 1.0f);
        outL[i] = (int32_t)(v*2147483648.0f);
        outR[i] = outL[i];
    }
}

// These three functions are rarely used, but ASIO requires
void sampleRateDidChange(ASIOSampleRate sRate) {} // When sample rate changes
long asioMessage(long selector, long value, void* message, double* opt) { return 0; } // ASIO messages
ASIOTime* bufferSwitchTimeInfo(ASIOTime* params, long index, ASIOBool processNow) { return nullptr; } // Bufferswitch advanced version

AsioDrivers drivers;

int main() {
    char names[32][32] = {};
    char* ptrs[32];
    for (int i = 0; i < 32; i++) ptrs[i] = names[i];

    long n = drivers.getDriverNames(ptrs, 32);
    std::cout << "Found " << n << "driver(s):" << std::endl;
    for (long i = 0; i < n; i++) {
        std::cout << " [" << i << "]" << names[i] << std::endl;
    }

    if (n == 0) {
        std::cout << "No ASIO driver installed." << std::endl;
        return -1;
    }

    if (!drivers.loadDriver(names[1])) {
        std::cout << "loadDriver failed: " << names[1] << std::endl;
        return -1;
    }

    // Driver Initialization
    driverInfo.sysRef = GetForegroundWindow();  // Windows handle
    if (ASIOInit(&driverInfo) != ASE_OK) {
        std::cout << "ASIOInit failed: " << driverInfo.errorMessage << std::endl;
        return -1;
    }
    std::cout << "Driver: " << driverInfo.name << std::endl;

    /*
    ASIOChannelInfo ci = {};
    ci.channel = 0;
    ci.isInput = ASIOTrue;
    ASIOError err = ASIOGetChannelInfo(&ci);
    std::cout << "err=" << err << " type=" << ci.type
            << " name=" << ci.name << " active=" << ci.isActive << std::endl;
    */

    bufferInfos[0].isInput = ASIOTrue; // Input buffer
    bufferInfos[0].channelNum = 0; // channel 0 (Guitar)
    bufferInfos[1].isInput = ASIOFalse; // output
    bufferInfos[1].channelNum = 0; 
    bufferInfos[2].isInput = ASIOFalse; // output
    bufferInfos[2].channelNum = 1; 

    callbacks.bufferSwitch = bufferSwitch;
    callbacks.sampleRateDidChange = sampleRateDidChange;
    callbacks.asioMessage = asioMessage;
    callbacks.bufferSwitchTimeInfo = bufferSwitchTimeInfo;

    ASIOCreateBuffers(bufferInfos, 3, bufferSize, &callbacks);
    /*
    Callback registration (bufferSwitch function)
    3: input 1, output 2
    */

    // Start
    board.nameprint();
    ASIOStart();
    std::cout << "Pedal on" << std::endl;
    std::cout << "Press enter to quit" << std::endl;

    std::cin.get();
    std::cout << "Pedal off" << std::endl;

    // Finish
    ASIOStop();
    ASIODisposeBuffers();
    drivers.removeCurrentDriver();
    return 0;

}