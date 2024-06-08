cmake_minimum_required(VERSION 3.28)

set(expat_folder "3rdParty/expat")
set(expat_SOURCE_DIR "${CMAKE_SOURCE_DIR}/${expat_folder}")

file(GLOB_RECURSE EXPAT_SOURCE_FILES
    "${expat_SOURCE_DIR}/Source/*.*")

source_group(TREE "${expat_SOURCE_DIR}" FILES 
    ${EXPAT_SOURCE_FILES})

add_library(expat ${EXPAT_SOURCE_FILES})

target_compile_definitions(expat PRIVATE COMPILED_FROM_DSP)
target_compile_definitions(expat PUBLIC XML_STATIC)

target_include_directories(expat PUBLIC "${expat_SOURCE_DIR}/Source/lib")

set_target_properties(expat PROPERTIES FOLDER ${expat_folder})
