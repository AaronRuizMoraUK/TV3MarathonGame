cmake_minimum_required(VERSION 3.28)

set(lua_folder "3rdParty/lua")
set(lua_SOURCE_DIR "${CMAKE_SOURCE_DIR}/${lua_folder}")

file(GLOB_RECURSE LUA_SOURCE_FILES
    "${lua_SOURCE_DIR}/lua/*.*")

source_group(TREE "${lua_SOURCE_DIR}" FILES 
    ${LUA_SOURCE_FILES})

add_library(lua ${LUA_SOURCE_FILES})

target_include_directories(lua PUBLIC "${lua_SOURCE_DIR}/lua")

set_target_properties(lua PROPERTIES FOLDER ${lua_folder})
