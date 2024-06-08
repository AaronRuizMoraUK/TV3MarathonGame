cmake_minimum_required(VERSION 3.28)

set(solid3_folder "3rdParty/solid3")
set(solid3_SOURCE_DIR "${CMAKE_SOURCE_DIR}/${solid3_folder}")

file(GLOB_RECURSE SOLID3_SOURCE_FILES
    "${solid3_SOURCE_DIR}/include/*.*"
    "${solid3_SOURCE_DIR}/src/*.*")

source_group(TREE "${solid3_SOURCE_DIR}" FILES 
    ${SOLID3_SOURCE_FILES})

add_library(solid3 ${SOLID3_SOURCE_FILES})

target_include_directories(solid3 PUBLIC "${solid3_SOURCE_DIR}/include") 
target_include_directories(solid3 PRIVATE "${solid3_SOURCE_DIR}/src") 
target_include_directories(solid3 PRIVATE "${solid3_SOURCE_DIR}/src/complex") 
target_include_directories(solid3 PRIVATE "${solid3_SOURCE_DIR}/src/convex") 

set_target_properties(solid3 PROPERTIES FOLDER ${solid3_folder})
