#pragma once
#include <string>
#include <vector>
#include <mmdeviceapi.h>

struct Device {
    std::wstring name;
    IMMDevice* device;
};

extern std::vector<Device> gAudioDevices;

std::vector<Device> getInputDevices();   // mics
std::vector<Device> getOutputDevices();  // speakers/headphones

int getDefaultDeviceIndex(EDataFlow flow, const std::vector<Device>& devices);

void playTestAudio(int deviceIndex);