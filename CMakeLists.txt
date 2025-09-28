cmake_minimum_required(VERSION 3.10)
project(ClippingSoftware)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(cmake/imgui)

add_definitions(-DUNICODE -D_UNICODE)

add_executable(ClippingSoftware 
    # My source
    src/gui/main.cpp 
    src/logic/clipping_storage.cpp 
    src/logic/file_handler.cpp 
    src/logic/settings_loader.cpp 
    src/logic/setup_hotkey.cpp 
    src/logic/screen_capture.cpp

    # ImGui source
    vendors/imguiFileDialog/ImGuiFileDialog.cpp 

    # libdshowcapture source
    vendors/libdshowcapture/source/capture-filter.cpp
    vendors/libdshowcapture/source/output-filter.cpp
    vendors/libdshowcapture/source/dshowcapture.cpp
    vendors/libdshowcapture/source/dshowencode.cpp
    vendors/libdshowcapture/source/device.cpp
    vendors/libdshowcapture/source/encoder.cpp
    vendors/libdshowcapture/source/dshow-base.cpp
    vendors/libdshowcapture/source/dshow-demux.cpp
    vendors/libdshowcapture/source/dshow-enum.cpp
    vendors/libdshowcapture/source/dshow-formats.cpp
    vendors/libdshowcapture/source/dshow-media-type.cpp
    vendors/libdshowcapture/source/dshow-encoded-device.cpp
    vendors/libdshowcapture/source/log.cpp
    vendors/libdshowcapture/source/obs_stubs.cpp
)

target_include_directories(ClippingSoftware PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendors/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/vendors/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/vendors/imguiFileDialog
    ${CMAKE_CURRENT_SOURCE_DIR}/vendors/libdshowcapture/
    ${CMAKE_CURRENT_SOURCE_DIR}/vendors/
)

target_link_libraries(ClippingSoftware PRIVATE 
    imgui
    d3d11 
    dxgi 
    d3dcompiler 
    dwmapi
    strmiids
    setupapi
    ksuser
    winmm
    wmcodecdspuuid
)
