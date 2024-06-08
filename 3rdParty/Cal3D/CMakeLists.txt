cmake_minimum_required(VERSION 3.28)

set(cal3d_folder "3rdParty/Cal3D")
set(cal3d_SOURCE_DIR "${CMAKE_SOURCE_DIR}/${cal3d_folder}")

file(GLOB_RECURSE CAL3D_SOURCE_FILES
    "${cal3d_SOURCE_DIR}/src/*.*")

source_group(TREE "${cal3d_SOURCE_DIR}/src" FILES 
    ${CAL3D_SOURCE_FILES})

add_library(Cal3D ${CAL3D_SOURCE_FILES})

target_include_directories(Cal3D PUBLIC "${cal3d_SOURCE_DIR}/src") 

set_target_properties(Cal3D PROPERTIES FOLDER ${cal3d_folder})
