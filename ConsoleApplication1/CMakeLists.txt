cmake_minimum_required(VERSION 3.15)
project(ConsoleApplication1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app
    ConsoleApplication1/ConsoleApplication1.cpp
)

if (WIN32)
    target_include_directories(app PRIVATE
        ${CMAKE_SOURCE_DIR}/packages/raylib.5.5.0/build/native/include
    )
    target_link_libraries(app PRIVATE raylib)
else()
    find_package(raylib REQUIRED)
    target_link_libraries(app PRIVATE raylib)
endif()

