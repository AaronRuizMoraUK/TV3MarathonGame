cmake_minimum_required(VERSION 3.28)

include(FetchContent)

# ---------------------------------------------------------------------
# dxsdk-d3dx
#
# Redistributable package for the legacy DirectX SDK's D3DX9, D3DX10
# and/or D3DX11 utility libraries.
#
# https://www.nuget.org/api/v2/package/Microsoft.DXSDK.D3DX/9.29.952.8
# ---------------------------------------------------------------------

FetchContent_Declare(
    dxsdkd3dx
    URL "${CMAKE_SOURCE_DIR}/3rdParty/dxsdk-d3dx"
    URL_HASH SHA512=9f6a95ed858555c1c438a85219ede32c82729068b21dd7ecf11de01cf3cdd525b2f04a58643bfcc14c48a29403dc1c80246f0a12a1ef4377b91b855f6d6d7986
)

FetchContent_MakeAvailable(dxsdkd3dx)

file(ARCHIVE_EXTRACT INPUT "${dxsdkd3dx_SOURCE_DIR}/microsoft.dxsdk.d3dx.9.29.952.8.nupkg" DESTINATION ${dxsdkd3dx_SOURCE_DIR})

add_library(Microsoft::D3DX9 SHARED IMPORTED GLOBAL)

set_target_properties(Microsoft::D3DX9 PROPERTIES
    IMPORTED_LOCATION_RELEASE            "${dxsdkd3dx_SOURCE_DIR}/build/native/release/bin/x64/D3DX9_43.dll"
    IMPORTED_IMPLIB_RELEASE              "${dxsdkd3dx_SOURCE_DIR}/build/native/release/lib/x64/d3dx9.lib"
    IMPORTED_LOCATION_DEBUG              "${dxsdkd3dx_SOURCE_DIR}/build/native/debug/bin/x64/D3DX9d_43.dll"
    IMPORTED_IMPLIB_DEBUG                "${dxsdkd3dx_SOURCE_DIR}/build/native/debug/lib/x64/d3dx9d.lib"
    INTERFACE_INCLUDE_DIRECTORIES        "${dxsdkd3dx_SOURCE_DIR}/build/native/include"
    IMPORTED_CONFIGURATIONS              "Debug;Release"
    IMPORTED_LINK_INTERFACE_LANGUAGES    "C")
