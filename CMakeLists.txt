cmake_minimum_required(VERSION 3.15.0)

project(GuGuEngine)

add_subdirectory("src")
add_subdirectory("thirdParty/freetype-2.13.2")
add_subdirectory("game")
if(WIN32)
    add_subdirectory("editor")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Assimp build tests" FORCE)
    set(ASSIMP_INSTALL OFF CACHE BOOL "Assimp install" FORCE)
    set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "Assimp install PDB" FORCE)
    add_subdirectory("thirdParty/assimp")
endif()