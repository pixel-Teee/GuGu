cmake_minimum_required(VERSION 3.15.0)

file(GLOB_RECURSE LuaSourceFile "lua-5.4.6/*.h" "lua-5.4.6/*.c")

add_library(Lua STATIC ${LuaSourceFile})

target_include_directories(Lua PUBLIC "lua-5.4.6/src")