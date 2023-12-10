cmake_minimum_required(VERSION 3.15.0)

file(GLOB SourceFile "main.cpp"
"Window/*.h"
"Window/*.cpp"
"Application/*.h"
"Application/*.cpp"
"main.cpp"
"pch.h")

if(WIN32)
    message("[message]:this platform is windows!")
    file(GLOB PlatformSourceFile "Window/Platform/Windows/*.h" "Window/Platform/Windows/*.cpp" "Application/Platform/Windows/*.h" "Application/Platform/Windows/*.cpp")
    list(APPEND SourceFile ${PlatformSourceFile})
    add_executable(GuGu WIN32 ${SourceFile})
    target_include_directories(GuGu PRIVATE "." PRIVATE "Window" PRIVATE "Application")
    add_definitions(-DUNICODE -D_UNICODE)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        message("[message]:set msvc utf8 successful!")
	    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
	    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    endif()
elseif(ANDROID)
    message("[message]:this platform is android!")
    file(GLOB PlatformSourceFile "Window/Platform/Android/*.h" "Window/Platform/Android/*.cpp" "Application/Platform/Android/*.h" "Application/Platform/Android/*.cpp")
    list(APPEND SourceFile ${PlatformSourceFile})
    add_library(GuGu SHARED ${SourceFile})
    target_include_directories(GuGu PRIVATE "." PRIVATE "Window" PRIVATE "Application")

    find_package(game-activity)
    find_library(log-lib log) #ndk library
    target_link_libraries(GuGu android game-activity::game-activity ${log-lib})  
endif()

target_precompile_headers(GuGu PRIVATE "pch.h")