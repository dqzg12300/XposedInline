

cmake_minimum_required(VERSION 3.4.1)
set(DobbyHome Dobby)
include_directories(
        ${DobbyHome}/include
        ${DobbyHome}/external/logging
)

add_subdirectory(Substrate)
add_library(
        native-libs
        SHARED
        dlfc/dlfcn_nougat.cpp
        utils/parse.cpp
        main.cpp
        utils/pmparser.c
        utils/wrap.cpp
        utils/hide.cpp
        fix.cpp
        )


find_library(
        log-lib
        log )

macro(SET_OPTION option value)
        set(${option} ${value} CACHE INTERNAL "" FORCE)
endmacro()
SET_OPTION(DOBBY_DEBUG ON)
SET_OPTION(DOBBY_GENERATE_SHARED OFF)
SET_OPTION(DynamicBinaryInstrument ON)
SET_OPTION(NearBranch ON)
SET_OPTION(Plugin.SymbolResolver ON)
add_subdirectory(${DobbyHome} dobby)

target_link_libraries(
        native-libs
        ${log-lib} substrate dobby)