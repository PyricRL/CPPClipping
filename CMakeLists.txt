cmake_minimum_required(VERSION 3.10)
project(ClippingSoftware)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(cmake/imgui)

add_executable(ClippingSoftware src/gui/main.cpp src/logic/clipping_storage.cpp)

target_link_libraries(ClippingSoftware PRIVATE imgui)

# Optional: include your src and vendor folders
target_include_directories(ClippingSoftware PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendors/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/vendors/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/vendors/imFileDialog
    ${CMAKE_CURRENT_SOURCE_DIR}/vendors/
)

# For Win32 + DX11 backend
target_link_libraries(ClippingSoftware PRIVATE d3d11 dxgi d3dcompiler dwmapi)