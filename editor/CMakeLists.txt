cmake_minimum_required(VERSION 3.15.0)

file(GLOB SourceFile "main.cpp")

if(WIN32)
	add_executable(Editor WIN32 ${SourceFile})
elseif(ANDROID)
	add_library(Editor SHARED ${SourceFile})
endif()

target_link_libraries(Editor PUBLIC GuGu)
target_include_directories(Editor PUBLIC "../src")
target_precompile_headers(Editor PRIVATE "../src/pch.h")
target_compile_features(Editor PRIVATE cxx_std_17)
add_dependencies(Editor GuGu)