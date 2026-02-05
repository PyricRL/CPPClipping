#include "audio_handler.hpp"
#include <cstddef>
#include <functiondiscoverykeys_devpkey.h>
#include <audioclient.h>
#include <minwindef.h>
#include <psapi.h>
#include <string>
#include <vector>
#include <windows.h>
#include <winnt.h>

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

        device->AddRef();

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

        device->AddRef();

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
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    if (deviceIndex < 0 || deviceIndex >= (int)gAudioDevices.size()) {
        std::cout << "[Audio] ERROR: deviceIndex out of range\n";
        return;
    }

    IMMDevice* device = gAudioDevices[deviceIndex].device;
    if (!device) {
        std::cout << "[Audio] ERROR: device is null\n";
        return;
    }

    IAudioClient* audioClient = nullptr;
    IAudioRenderClient* renderClient = nullptr;
    WAVEFORMATEX* pwfx = nullptr;

    HRESULT hr;
    hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&audioClient);
    if (FAILED(hr)) {
        std::cout << "[Audio] ERROR: Activate failed\n";
        return;
    }

    hr = audioClient->GetMixFormat(&pwfx);
    if (FAILED(hr) || !pwfx) {
        std::cout << "[Audio] ERROR: GetMixFormat failed\n";
        audioClient->Release();
        return;
    }

    bool isFloat = false;

    if (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
        isFloat = true;
    }
    else if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        auto* wfe = (WAVEFORMATEXTENSIBLE*)pwfx;
        if (wfe->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
            isFloat = true;
    }

    if (!isFloat) {
        std::cout << "[Audio] ERROR: Unsupported format\n";
        CoTaskMemFree(pwfx);
        audioClient->Release();
        return;
    }
    hr = audioClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        0,
        10000000,
        0,
        pwfx,
        nullptr
    );

    if (FAILED(hr)) {
        std::cout << "[Audio] ERROR: Initialize failed hr=0x" << std::hex << hr << std::dec << "\n";
        CoTaskMemFree(pwfx);
        audioClient->Release();
        return;
    }

    hr = audioClient->GetService(__uuidof(IAudioRenderClient), (void**)&renderClient);
    if (FAILED(hr) || !renderClient) {
        std::cout << "[Audio] ERROR: GetService failed\n";
        CoTaskMemFree(pwfx);
        audioClient->Release();
        return;
    }

    UINT32 bufferFrames = 0;
    audioClient->GetBufferSize(&bufferFrames);

    audioClient->Start();

    UINT32 padding = 0;
    audioClient->GetCurrentPadding(&padding);
    UINT32 framesToWrite = bufferFrames - padding;

    if (framesToWrite == 0) {
        std::cout << "[Audio] ERROR: No space in buffer\n";
        audioClient->Stop();
        CoTaskMemFree(pwfx);
        renderClient->Release();
        audioClient->Release();
        return;
    }

    BYTE* data = nullptr;
    hr = renderClient->GetBuffer(framesToWrite, &data);
    if (FAILED(hr) || !data) {
        std::cout << "[Audio] ERROR: GetBuffer failed\n";
        audioClient->Stop();
        CoTaskMemFree(pwfx);
        renderClient->Release();
        audioClient->Release();
        return;
    }

    float* samples = (float*)data;
    int channels = pwfx->nChannels;
    int rate = pwfx->nSamplesPerSec;

    static float phase = 0.0f;

    for (UINT32 i = 0; i < framesToWrite; i++) {
        float v = sinf(phase) * 0.2f;
        for (int c = 0; c < channels; c++)
            samples[i * channels + c] = v;

        phase += 2.0f * 3.14159265f * 440.0f / rate;
        if (phase > 2.0f * 3.14159265f)
            phase -= 2.0f * 3.14159265f;
    }

    renderClient->ReleaseBuffer(framesToWrite, 0);

    Sleep(1000);

    audioClient->Stop();

    CoTaskMemFree(pwfx);
    renderClient->Release();
    audioClient->Release();
}

std::vector<Application> getApplicationIDs() {
    std::vector<Application> apps;
    DWORD pids[1024];
    DWORD cb = 1024 * sizeof(DWORD);
    DWORD cbNeeded = 0;

    if (!EnumProcesses(pids, cb, &cbNeeded)) {
        std::cout << ("Failed") << std::endl;
        
        return apps;
    }

    DWORD numPIDs = cbNeeded / sizeof(DWORD);
    for (DWORD i = 0; i < numPIDs; ++i) {
        printf("PID: %lu\n", pids[i]);
        if (pids[i] == 0) continue;

        HANDLE handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, pids[i]);

        if (handle != NULL) {
            WCHAR exePath[MAX_PATH];
            DWORD size = MAX_PATH;
            if (QueryFullProcessImageNameW(handle, 0, exePath, &size)) {
                // compare only the executable name (case-insensitive)
                const wchar_t* exeName = wcsrchr(exePath, L'\\');
                if (exeName) exeName++; // skip the backslash
                std::wstring wstr(exeName);
                std::string str(wstr.begin(), wstr.end());

                Application tempApp = {str, pids[i]};
                apps.push_back(tempApp);
            }
        }
    }
    return apps;
}