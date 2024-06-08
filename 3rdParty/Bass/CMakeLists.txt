cmake_minimum_required(VERSION 3.28)

set(bass_folder "3rdParty/Bass")
set(bass_SOURCE_DIR "${CMAKE_SOURCE_DIR}/${bass_folder}")

add_library(BassShared SHARED IMPORTED GLOBAL)

set_target_properties(BassShared PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES        "${bass_SOURCE_DIR}/c"
    INTERFACE_LINK_DIRECTORIES           "${bass_SOURCE_DIR}/x64"
    IMPORTED_LOCATION                    "${bass_SOURCE_DIR}/x64/bass.dll"
    IMPORTED_IMPLIB                      "${bass_SOURCE_DIR}/x64/bass.lib")

# Additional interface target to see bass as a project in IDE
file(GLOB_RECURSE BASS_SOURCE_FILES
    "${bass_SOURCE_DIR}/c/*.*")

source_group(TREE "${bass_SOURCE_DIR}/c" FILES 
    ${BASS_SOURCE_FILES})

add_library(Bass INTERFACE ${BASS_SOURCE_FILES})

set_target_properties(Bass PROPERTIES FOLDER ${bass_folder})
