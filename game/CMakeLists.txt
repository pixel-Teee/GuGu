cmake_minimum_required(VERSION 3.15.0)

file(GLOB SourceFile "main.cpp")

if(WIN32)
	add_executable(Game WIN32 ${SourceFile})
elseif(ANDROID)
	add_library(Game SHARED ${SourceFile})
endif()

target_link_libraries(Game PUBLIC GuGu)
target_include_directories(Game PUBLIC "../src")
target_precompile_headers(Game PRIVATE "../src/pch.h")
target_compile_features(Game PRIVATE cxx_std_17)
add_dependencies(Game GuGu)