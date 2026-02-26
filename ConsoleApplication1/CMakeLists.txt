cmake_minimum_required(VERSION 3.15)   # MUST be first
project(Invaders LANGUAGES CXX)         # MUST be after cmake_minimum_required

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories (raygui.h / raylib headers)
include_directories(${CMAKE_SOURCE_DIR}/include)  # your raygui.h
if(WIN32)
    include_directories(${CMAKE_SOURCE_DIR}/ConsoleApplication1/packages/raylib.5.5.0/build/native/include)
endif()

# Add executable
add_executable(app
    ConsoleApplication1/ConsoleApplication1.cpp
    ConsoleApplication1/Player.cpp
)

# Link libraries
if(WIN32)
    target_link_libraries(app PRIVATE raylib)
else()
    find_package(raylib REQUIRED)
    target_link_libraries(app PRIVATE raylib)
endif()

