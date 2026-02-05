#pragma once
#include "iostream"
#include <string>
#include <vector>
#include <mmdeviceapi.h>

struct Device {
    std::wstring name;
    IMMDevice* device;
};

struct Application {
    std::string name;
    DWORD pid;
};

extern std::vector<Device> gAudioDevices;

std::vector<Device> getInputDevices();   // mics
std::vector<Device> getOutputDevices();  // speakers/headphones

int getDefaultDeviceIndex(EDataFlow flow, const std::vector<Device>& devices);

void playTestAudio(int deviceIndex);

std::vector<Application> getApplicationIDs();