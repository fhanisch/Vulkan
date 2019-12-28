cmake_minimum_required(VERSION 3.10)

project(VulkanApp)

add_definitions(-DWINDOWS)

include_directories(C:/VulkanSDK/1.1.126.0/Include)

add_executable(app.exe src/main.cpp src/Window.cpp src/VulkanSetup.cpp src/Models.cpp src/matrix.c)