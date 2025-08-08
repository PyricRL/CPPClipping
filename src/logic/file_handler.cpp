#include <windows.h>
#include <shlobj.h>  // For SHGetKnownFolderPath
#include <string>

std::wstring GetVideosFolder() {
    PWSTR path = NULL;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Videos, 0, NULL, &path);
    std::wstring videosPath;
    if (SUCCEEDED(hr)) {
        videosPath = path;
    }
    CoTaskMemFree(path);
    return videosPath;
}

std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return {};

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, nullptr, nullptr);
    return strTo;
}

