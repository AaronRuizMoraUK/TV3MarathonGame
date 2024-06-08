cmake_minimum_required(VERSION 3.28)

add_subdirectory(Cal3D)
add_subdirectory(solid3)
add_subdirectory(Bass)
add_subdirectory(Particles)
add_subdirectory(lua)
add_subdirectory(expat)

include("FetchLibraries.cmake")
