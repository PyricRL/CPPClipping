#include "../vendors/libdshowcapture/dshowcapture.hpp"
#include "iostream"
#include "source/dshow-enum.hpp"
#include <string>
#include <vector>

struct AudioDeviceLists {
    std::vector<std::wstring> mics;
    std::vector<std::wstring> outputs;
};

AudioDeviceLists getAudioDevice() {
    std::vector<DShow::AudioDevice> audioDevices;
    std::vector<DShow::VideoDevice> micDevices;
    AudioDeviceLists result;

    std::cout << "Audio Devices:" << std::endl;
    if (DShow::Device::EnumAudioOutputs(audioDevices)) {
        for (auto &dev : audioDevices) {
            result.outputs.push_back(dev.name);
            std::wcout << dev.name << std::endl;
        }
    } else {
        std::cout << "Failed to enumerate audio output devices\n";
    }

    std::cout << "Video/Mic Devices:" << std::endl;
    if (DShow::Device::EnumVideoDevices(micDevices)) {
        for (auto &dev : micDevices) {
            result.outputs.push_back(dev.name);
            std::wcout << dev.name << std::endl;
        }
    } else {
        std::cout << "Failed to enumerate mic output devices\n";
    }

    return result;
}