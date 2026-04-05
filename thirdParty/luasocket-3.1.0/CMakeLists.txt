cmake_minimum_required(VERSION 3.15.0)

set(LuaSocketSourceFile
    ${CMAKE_CURRENT_SOURCE_DIR}/src/luasocket.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mime.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/auxiliar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/compat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/except.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inet.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/options.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/select.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/timeout.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/udp.c
)

if(WIN32)
    list(APPEND LuaSocketSourceFile ${CMAKE_CURRENT_SOURCE_DIR}/src/wsocket.c)
else()
    list(APPEND LuaSocketSourceFile ${CMAKE_CURRENT_SOURCE_DIR}/src/usocket.c)
endif()

add_library(LuaSocket STATIC ${LuaSocketSourceFile})

target_include_directories(LuaSocket PRIVATE "src" "../lua/lua-5.4.6/src")

if(WIN32)
    target_link_libraries(LuaSocket ws2_32)
endif()



