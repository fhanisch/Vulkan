cmake_minimum_required(VERSION 3.10)

project(VulkanApp)

add_definitions(-DWINDOWS)

include_directories(C:/VulkanSDK/1.1.126.0/Include)

file(GLOB SOURCES "src/*.cpp" "src/*.c" "src/*.h")

add_executable(app.exe ${SOURCES})