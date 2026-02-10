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
"Core/UI/*.h"
"Core/UI/*.cpp"
"Core/Math/*.h"
"Core/Math/*.cpp"
"Core/SceneGraph/*.h"
"Core/SceneGraph/*.cpp"
"Core/FileSystem/*.h"
"Core/FileSystem/*.cpp"
"Core/Animation/*.h"
"Core/Animation/*.cpp"
"Core/Reflection/*.h"
"Core/Reflection/*.cpp"
"Core/Reflection/*.hpp"
"Core/Reflection/MetaProperty/*.h"
"Core/Reflection/MetaProperty/*.cpp"
"Core/GamePlay/*.h"
"Core/GamePlay/*.cpp"
"Core/GamePlay/GameUI/*.h"
"Core/GamePlay/GameUI/*.cpp"
"Core/GamePlay/Delegate/*.h"
"Core/GamePlay/Delegate/*.cpp"
"Core/GamePlay/RenderComponent/*.h"
"Core/GamePlay/RenderComponent/*.cpp"
"Core/GamePlay/Physics/*.h"
"Core/GamePlay/Physics/*.cpp"
"Core/AssetManager/*.h"
"Core/AssetManager/*.cpp"
"Core/Model/*.h"
"Core/Model/*.cpp"
"Core/Collision/*.h"
"Core/Collision/*.cpp"
"Core/Texture/*.h"
"Core/Texture/*.cpp"
"gltf/*.h"
"gltf/*.cpp"
"pch.h"
"Core/LuaContext/*.h"
"Core/LuaContext/*.cpp"
"Core/GamePlay/2D/*.h"
"Core/GamePlay/2D/*.cpp"
"Core/Physics/*.h"
"Core/Physics/*.cpp"
"Core/Debug/*.h"
"Core/Debug/*.cpp")

if(WIN32)
    message("[message]:this platform is windows!")

    set(USE_VK 1)
    set(USE_DX12 0)

    if(USE_DX12) 
        file(GLOB PlatformSourceFile "Window/Platform/Windows/*.h" "Window/Platform/Windows/*.cpp" "Application/Platform/Windows/*.h" "Application/Platform/Windows/*.cpp"
        "Renderer/Platform/D3D12/*.h" "Renderer/Platform/D3D12/*.cpp" "Core/Platform/Windows/*.h" "Core/Platform/Windows/*.cpp")
        list(APPEND SourceFile ${PlatformSourceFile})
        source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFile})

        add_library(GuGu ${SourceFile})
        target_include_directories(GuGu PRIVATE "." PRIVATE "Window" PRIVATE "Application" PRIVATE "Renderer" PRIVATE "Core" PRIVATE "../thirdParty/nlohmann" PRIVATE "../thirdParty/bullet3-3.17/src")
        target_link_libraries(GuGu freetype Lua)
        add_definitions(-DUSE_DX12)
    elseif(USE_VK)
        file(GLOB PlatformSourceFile "Window/Platform/Windows/*.h" "Window/Platform/Windows/*.cpp" "Application/Platform/Windows/*.h" "Application/Platform/Windows/*.cpp"
        "Renderer/Platform/Vulkan/*.h" "Renderer/Platform/Vulkan/*.cpp" "Core/Platform/Windows/*.h" "Core/Platform/Windows/*.cpp")
        #message(${PlatformSourceFile})
        find_package(Vulkan REQUIRED)
        list(APPEND SourceFile ${PlatformSourceFile})
        source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFile})

        add_library(GuGu ${SourceFile} ${ThirdPartyFile})
        target_include_directories(GuGu PRIVATE "." PRIVATE "Window" PRIVATE "Application" PRIVATE "Renderer" PRIVATE "Core" PRIVATE "../thirdParty/nlohmann" PRIVATE "../thirdParty/bullet3-3.17/src")
        target_link_libraries(GuGu Vulkan::Vulkan freetype Lua Bullet3Common BulletDynamics BulletCollision LinearMath)
        add_definitions(-DUSE_VK)
    endif()

    # file(GLOB PlatformSourceFile "Window/Platform/Windows/*.h" "Window/Platform/Windows/*.cpp" "Application/Platform/Windows/*.h" "Application/Platform/Windows/*.cpp"
    # "Renderer/Platform/D3D12/*.h" "Renderer/Platform/D3D12/*.cpp" "Core/Platform/Windows/*.h" "Core/Platform/Windows/*.h" "Core/Platform/Windows/*.h" "Core/Platform/Windows/*.cpp")
    # list(APPEND SourceFile ${PlatformSourceFile})
    # add_executable(GuGu WIN32 ${SourceFile})
    # target_include_directories(GuGu PRIVATE "." PRIVATE "Window" PRIVATE "Application" PRIVATE "Renderer" PRIVATE "Core")
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
    # set(CMAKE_BUILD_TYPE Debug)
    find_package(Vulkan REQUIRED)
    # message("hello")
    file(GLOB PlatformSourceFile "Window/Platform/Android/*.h" "Window/Platform/Android/*.cpp" "Application/Platform/Android/*.h" "Application/Platform/Android/*.cpp"
    "Renderer/Platform/Vulkan/*.h" "Renderer/Platform/Vulkan/*.cpp"
    "Core/Platform/Android/*.h" "Core/Platform/Android/*.cpp")
    #message(${PlatformSourceFile})
    list(APPEND SourceFile ${PlatformSourceFile})

    source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${ThirdPartyFile})

    add_library(GuGu STATIC ${SourceFile} ${ThirdPartyFile})
    target_include_directories(GuGu PRIVATE "." PRIVATE "Window" PRIVATE "Application" PRIVATE "Renderer" PRIVATE "Core" PRIVATE "../thirdParty/nlohmann" PRIVATE "../thirdParty/bullet3-3.17/src")
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR=TRUE) #fix this
    add_definitions(-DUSE_VK)
    add_definitions(-D_DEBUG=TRUE)
    find_package(game-activity REQUIRED CONFIG)
    find_library(log-lib log) #ndk library
    target_link_libraries(GuGu android game-activity::game-activity_static ${log-lib} vulkan freetype Lua Bullet3Common BulletDynamics BulletCollision LinearMath)
    target_compile_features(GuGu PRIVATE cxx_std_17)
endif()

target_precompile_headers(GuGu PRIVATE "pch.h")