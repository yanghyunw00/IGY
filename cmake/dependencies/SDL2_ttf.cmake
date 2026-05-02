add_library(the_sdl2_ttf INTERFACE)

if(WIN32)
    FetchContent_Declare(
        sdl2_ttf
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-devel-2.22.0-VC.zip
    )
    FetchContent_MakeAvailable(sdl2_ttf)

    target_include_directories(the_sdl2_ttf SYSTEM INTERFACE ${sdl2_ttf_SOURCE_DIR}/include)
    target_link_directories(the_sdl2_ttf INTERFACE ${sdl2_ttf_SOURCE_DIR}/lib/x64)
    target_link_libraries(the_sdl2_ttf INTERFACE SDL2_ttf SDL2)

    set(TEMP_EXE_FOLDER $<IF:$<BOOL:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}>,${CMAKE_RUNTIME_OUTPUT_DIRECTORY},${CMAKE_BINARY_DIR}>)

    file(GLOB SDL2_TTF_DLLS ${sdl2_ttf_SOURCE_DIR}/lib/x64/*.dll)
    add_custom_target(copy_sdl2_ttf_dll
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${SDL2_TTF_DLLS}
                ${TEMP_EXE_FOLDER}
        DEPENDS ${SDL2_TTF_DLLS}
        COMMENT "Copying SDL2_ttf runtime libraries"
    )
    add_dependencies(the_sdl2_ttf copy_sdl2_ttf_dll)
else()
    if(EMSCRIPTEN)
        target_compile_options(the_sdl2_ttf INTERFACE --use-port=sdl2_ttf)
        target_link_options(the_sdl2_ttf INTERFACE --use-port=sdl2_ttf)
    else()
        find_package(SDL2_ttf REQUIRED)
        target_link_libraries(the_sdl2_ttf INTERFACE SDL2_ttf::SDL2_ttf)
    endif()
endif()
