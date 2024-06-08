cmake_minimum_required(VERSION 3.28)

set(papi_folder "3rdParty/Particles")
set(papi_SOURCE_DIR "${CMAKE_SOURCE_DIR}/${papi_folder}")

file(GLOB_RECURSE PAPI_SOURCE_FILES
    "${papi_SOURCE_DIR}/Papi/*.*")

source_group(TREE "${papi_SOURCE_DIR}" FILES 
    ${PAPI_SOURCE_FILES})

add_library(papi ${PAPI_SOURCE_FILES})

target_include_directories(papi PUBLIC "${papi_SOURCE_DIR}")

set_target_properties(papi PROPERTIES FOLDER ${papi_folder})
