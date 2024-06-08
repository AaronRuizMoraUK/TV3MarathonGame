cmake_minimum_required(VERSION 3.28)

file(GLOB_RECURSE TV3_SOURCE_FILES
    "${CMAKE_SOURCE_DIR}/Source/*.*")

source_group(TREE "${CMAKE_SOURCE_DIR}" FILES 
    ${TV3_SOURCE_FILES})

add_executable(TV3MarathonGame
    ${TV3_SOURCE_FILES})

# Includes
target_include_directories(TV3MarathonGame PRIVATE "${CMAKE_SOURCE_DIR}/Source")

# Libraries
target_link_libraries(TV3MarathonGame PRIVATE d3d9.lib)
target_link_libraries(TV3MarathonGame PRIVATE Microsoft::D3DX9)
target_link_libraries(TV3MarathonGame PRIVATE xinput.lib)
target_link_libraries(TV3MarathonGame PRIVATE dinput8.lib)
target_link_libraries(TV3MarathonGame PRIVATE dxguid.lib)
target_link_libraries(TV3MarathonGame PRIVATE Winmm.lib)
target_link_libraries(TV3MarathonGame PRIVATE Cal3D)
target_link_libraries(TV3MarathonGame PRIVATE solid3)
target_link_libraries(TV3MarathonGame PRIVATE BassShared)
target_link_libraries(TV3MarathonGame PRIVATE papi)
target_link_libraries(TV3MarathonGame PRIVATE lua)
target_link_libraries(TV3MarathonGame PRIVATE expat)

# Post Build commands to copy DLLs to TV3MarathonGame path
add_custom_command(TARGET TV3MarathonGame POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:BassShared> $<TARGET_FILE_DIR:TV3MarathonGame>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Microsoft::D3DX9> $<TARGET_FILE_DIR:TV3MarathonGame>)

# Set the working directory for the TV3MarathonGame target where it will find the data folder
set_property(TARGET TV3MarathonGame PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
