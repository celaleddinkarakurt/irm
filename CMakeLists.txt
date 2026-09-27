cmake_minimum_required(VERSION 3.15)
project(irm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(irm
    src/main.cpp
    src/terminal.cpp
    src/editor.cpp
    src/file_manager.cpp
    src/command.cpp
    src/app_state.cpp
    src/image_viewer.cpp
    src/mode.cpp
    src/application.cpp
    src/renderer.cpp
    src/layout.cpp
)