# author Rudy Castan
# date 2025 Fall
# CS200 Computer Graphics I
# copyright DigiPen Institute of Technology

cmake_minimum_required(VERSION 3.21)
set(ENV{VSLANG} 1033)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

# #  Helper to print out all environment variables
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

project(cs200_fun)

include(cmake/StandardProjectSettings.cmake)

include(cmake/CompilerWarnings.cmake)
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
set_project_warnings(project_options)

# Optional: Visual Leak Detector (VLD) support for Windows
option(USE_VLD "Use Visual Leak Detector for memory leak detection (Windows only)" OFF)
if(USE_VLD AND WIN32)
    find_path(VLD_INCLUDE_DIR vld.h HINTS "C:/Program Files (x86)/Visual Leak Detector/include")
    find_library(VLD_LIBRARY vld HINTS "C:/Program Files (x86)/Visual Leak Detector/lib/Win64")

    if(VLD_INCLUDE_DIR AND VLD_LIBRARY)
        message(STATUS "Visual Leak Detector found")
        message(STATUS "  Include: ${VLD_INCLUDE_DIR}")
        message(STATUS "  Library: ${VLD_LIBRARY}")
        target_compile_definitions(project_options INTERFACE USE_VLD)
        target_include_directories(project_options INTERFACE ${VLD_INCLUDE_DIR})
        target_link_libraries(project_options INTERFACE ${VLD_LIBRARY})
    else()
        message(WARNING "Visual Leak Detector requested but not found. Install from https://github.com/KindDragon/vld")
        message(WARNING "  Include found: ${VLD_INCLUDE_DIR}")
        message(WARNING "  Library found: ${VLD_LIBRARY}")
    endif()
endif()

include(cmake/Dependencies.cmake)

add_subdirectory(source)

