cmake_minimum_required(VERSION 3.15.0)

file(GLOB SourceFile "main.cpp"
"Window/*.h"
"Window/*.cpp"
"Application/*.h"
"Application/*.cpp"
"Renderer/*.h"
"Renderer/*.cpp"
"main.cpp"
"Core/*.h"
"Core/*.cpp"
"pch.h")

if(WIN32)
    message("[message]:this platform is windows!")
    file(GLOB PlatformSourceFile "Window/Platform/Windows/*.h" "Window/Platform/Windows/*.cpp" "Application/Platform/Windows/*.h" "Application/Platform/Windows/*.cpp"
    "Renderer/Platform/D3D12/*.h" "Renderer/Platform/D3D12/*.cpp" "Core/Platform/Windows/*.h" "Core/Platform/Windows/*.h" "Core/Platform/Windows/*.h" "Core/Platform/Windows/*.cpp")
    list(APPEND SourceFile ${PlatformSourceFile})
    add_executable(GuGu WIN32 ${SourceFile})
    target_include_directories(GuGu PRIVATE "." PRIVATE "Window" PRIVATE "Application" PRIVATE "Renderer" PRIVATE "Core")
    add_definitions(-DUNICODE -D_UNICODE -DWIN32)
    #if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    #    message("[message]:set msvc utf8 successful!")
	#    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
	#    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    #endif()
    set(CMAKE_CXX_FLAGS "/utf-8")
    set(CMAKE_C_FLAGS "/utf-8")
    target_compile_features(GuGu PRIVATE cxx_std_17)
elseif(ANDROID)
    message("[message]:this platform is android!")
    find_package(Vulkan REQUIRED)
    file(GLOB PlatformSourceFile "Window/Platform/Android/*.h" "Window/Platform/Android/*.cpp" "Application/Platform/Android/*.h" "Application/Platform/Android/*.cpp"
    "Renderer/Platform/Vulkan/*.h" "Renderer/Platform/Vulkan/*.cpp"
    "../thirdParty/vkbootstrap/*.h" "../thirdParty/vkbootstrap/*.cpp"
    "Core/Platform/Android/*.h" "Core/Platform/Android/*.cpp")
    #message(${PlatformSourceFile})
    list(APPEND SourceFile ${PlatformSourceFile})
    add_library(GuGu SHARED ${SourceFile})
    target_include_directories(GuGu PRIVATE "." PRIVATE "Window" PRIVATE "Application" PRIVATE "Renderer" PRIVATE "../thirdParty/vkbootstrap" PRIVATE "Core")
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR=TRUE) #fix this
    add_definitions(-DUSE_VK)
    add_definitions(-D_DEBUG)
    find_package(game-activity)
    find_library(log-lib log) #ndk library
    target_link_libraries(GuGu android game-activity::game-activity ${log-lib} vulkan)
    target_compile_features(GuGu PRIVATE cxx_std_17)
endif()

target_precompile_headers(GuGu PRIVATE "pch.h")