cmake_minimum_required(VERSION 3.15.0)

file(GLOB SourceFile "main.cpp" "Editor/*.*" "Editor/StyleSet/EditorStyleSet.*"
"Editor/ContentBrowser/*.*" "ModelImporter/*.*")

if(WIN32)
	add_executable(Editor WIN32 ${SourceFile})
elseif(ANDROID)
	add_library(Editor SHARED ${SourceFile})
endif()

#if(WIN32)
#	set(CMAKE_CXX_FLAGS "/utf-8")
#	set(CMAKE_C_FLAGS "/utf-8")
#endif()

if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE -DWIN32)
    set(CMAKE_CXX_FLAGS "/utf-8")
    set(CMAKE_C_FLAGS "/utf-8")
elseif(ANDROID)
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR=TRUE) #fix this
    add_definitions(-DUSE_VK)
    add_definitions(-D_DEBUG=TRUE)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFile})
target_link_libraries(Editor PUBLIC GuGu PRIVATE assimp)
target_include_directories(Editor PUBLIC "../src" "../editor" "../thirdParty/assimp/include")
target_precompile_headers(Editor PRIVATE "../src/pch.h")
target_compile_features(Editor PRIVATE cxx_std_17)
add_dependencies(Editor GuGu)