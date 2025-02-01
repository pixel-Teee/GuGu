cmake_minimum_required(VERSION 3.15.0)

file(GLOB SourceFile "main.cpp" "LevelViewport.*" "GameViewportClient.*")

if(WIN32)
	add_executable(Game WIN32 ${SourceFile})
elseif(ANDROID)
	add_library(Game SHARED ${SourceFile})
endif()

if(WIN32)
	add_definitions(-DUNICODE -D_UNICODE -DWIN32)
	set(CMAKE_CXX_FLAGS "/utf-8")
	set(CMAKE_C_FLAGS "/utf-8")
elseif(ANDROID)
	add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR=TRUE) #fix this
	add_definitions(-DUSE_VK)
	add_definitions(-D_DEBUG=TRUE)
endif()

target_link_libraries(Game PUBLIC GuGu)
target_include_directories(Game PUBLIC "../src")
target_precompile_headers(Game PRIVATE "../src/pch.h")
target_compile_features(Game PRIVATE cxx_std_17)
add_dependencies(Game GuGu)