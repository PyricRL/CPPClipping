file(GLOB IMGUI_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendors/imgui/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendors/imgui/backends/imgui_impl_win32.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendors/imgui/backends/imgui_impl_dx11.cpp
)

add_library(imgui STATIC ${IMGUI_SRC})

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendors/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendors/imgui/backends
)
