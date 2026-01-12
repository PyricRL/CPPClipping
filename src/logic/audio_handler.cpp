#include "audio_handler.hpp"
#include <functiondiscoverykeys_devpkey.h>
#include <audioclient.h>
#include <string>

std::vector<Device> getInputDevices() {
    std::vector<Device> devices;
    CoInitialize(nullptr);

    IMMDeviceEnumerator* enumerator = nullptr;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));

    IMMDeviceCollection* collection = nullptr;
    enumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &collection);

    UINT count;
    collection->GetCount(&count);

    for (UINT i = 0; i < count; ++i) {
        IMMDevice* device = nullptr;
        collection->Item(i, &device);

        IPropertyStore* props = nullptr;
        device->OpenPropertyStore(STGM_READ, &props);

        PROPVARIANT var;
        PropVariantInit(&var);
        props->GetValue(PKEY_Device_FriendlyName, &var);

        Device tempDevice = {var.pwszVal, device};

        devices.push_back(tempDevice);

        PropVariantClear(&var);
        props->Release();
        device->Release();
    }

    collection->Release();
    enumerator->Release();
    CoUninitialize();

    return devices;
}

std::vector<Device> getOutputDevices() {
    std::vector<Device> devices;
    CoInitialize(nullptr);

    IMMDeviceEnumerator* enumerator = nullptr;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));

    IMMDeviceCollection* collection = nullptr;
    enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &collection);

    UINT count;
    collection->GetCount(&count);

    for (UINT i = 0; i < count; ++i) {
        IMMDevice* device = nullptr;
        collection->Item(i, &device);

        IPropertyStore* props = nullptr;
        device->OpenPropertyStore(STGM_READ, &props);

        PROPVARIANT var;
        PropVariantInit(&var);
        props->GetValue(PKEY_Device_FriendlyName, &var);

        Device tempDevice = {var.pwszVal, device};

        devices.push_back(tempDevice);

        PropVariantClear(&var);
        props->Release();
        device->Release();
    }

    collection->Release();
    enumerator->Release();
    CoUninitialize();

    return devices;
}

int getDefaultDeviceIndex(EDataFlow flow, const std::vector<Device>& devices) {
    CoInitialize(nullptr);

    IMMDeviceEnumerator* enumerator = nullptr;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));

    IMMDevice* defaultDevice = nullptr;
    enumerator->GetDefaultAudioEndpoint(flow, eConsole, &defaultDevice);

    LPWSTR defaultName = nullptr;
    IPropertyStore* props = nullptr;
    defaultDevice->OpenPropertyStore(STGM_READ, &props);

    PROPVARIANT var;
    PropVariantInit(&var);
    props->GetValue(PKEY_Device_FriendlyName, &var);
    defaultName = var.pwszVal;

    int index = -1;
    for (size_t i = 0; i < devices.size(); ++i) {
        if (devices[i].name == defaultName) {
            index = static_cast<int>(i);
            break;
        }
    }

    PropVariantClear(&var);
    props->Release();
    defaultDevice->Release();
    enumerator->Release();
    CoUninitialize();

    return index; // -1 if not found
}

void playTestAudio(int deviceIndex) {
    IMMDevice* device = gAudioDevices[deviceIndex].device;
    IAudioClient* audioClient = nullptr;
    device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&audioClient);

    WAVEFORMATEX* pwfx = nullptr;
    audioClient->GetMixFormat(&pwfx); // gets the default format for the device

    audioClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        0,
        10000000,  // buffer duration in 100-ns units (here 1 second)
        0,
        pwfx,
        nullptr
    );

    IAudioRenderClient* renderClient = nullptr;
    audioClient->GetService(__uuidof(IAudioRenderClient), (void**)&renderClient);

    UINT32 bufferFrames;
    audioClient->GetBufferSize(&bufferFrames);

    audioClient->Start();

    // --- WRITE AUDIO ---
    BYTE* data = nullptr;
    renderClient->GetBuffer(bufferFrames, &data);

    float* samples = (float*)data;

    int channels = pwfx->nChannels;
    int sampleRate = pwfx->nSamplesPerSec;

    float freq = 440.0f; // A4
    static float phase = 0.0f;

    for (UINT32 i = 0; i < bufferFrames; i++) {
        float value = sinf(phase);

        for (int c = 0; c < channels; c++) {
            samples[i * channels + c] = value;
        }

        phase += 2.0f * 3.14159265f * freq / sampleRate;
        if (phase > 2.0f * 3.14159265f)
            phase -= 2.0f * 3.14159265f;
    }

    renderClient->ReleaseBuffer(bufferFrames, 0);

    Sleep(1000); // let it play

    audioClient->Stop();

    CoTaskMemFree(pwfx);
    renderClient->Release();
    audioClient->Release();
}