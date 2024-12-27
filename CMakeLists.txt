cmake_minimum_required(VERSION 3.15.0)

project(GuGuEngine)

add_subdirectory("src")
add_subdirectory("thirdParty/freetype-2.13.2")
add_subdirectory("game")
if(WIN32)
    add_subdirectory("editor")
endif()