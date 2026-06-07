# author Rudy Castan
# date 2025 Fall
# CS200 Computer Graphics I
# copyright DigiPen Institute of Technology

add_library(the_sdl2_mixer INTERFACE)

if(WIN32)
    FetchContent_Declare(
        sdl2_mixer
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.0/SDL2_mixer-devel-2.8.0-VC.zip
    )
    FetchContent_MakeAvailable(sdl2_mixer)

    target_include_directories(the_sdl2_mixer SYSTEM INTERFACE ${sdl2_mixer_SOURCE_DIR}/include)

    target_link_directories(the_sdl2_mixer INTERFACE ${sdl2_mixer_SOURCE_DIR}/lib/x64)
    target_link_libraries(the_sdl2_mixer INTERFACE SDL2_mixer SDL2)

    set(TEMP_EXE_FOLDER $<IF:$<BOOL:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}>,${CMAKE_RUNTIME_OUTPUT_DIRECTORY},${CMAKE_BINARY_DIR}>)

    file(GLOB SDL2_MIXER_DLLS ${sdl2_mixer_SOURCE_DIR}/lib/x64/*.dll)
    add_custom_target(copy_sdl2_mixer_dll
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${SDL2_MIXER_DLLS}
                ${TEMP_EXE_FOLDER}
        DEPENDS ${SDL2_MIXER_DLLS}
        COMMENT "Copying SDL2_mixer runtime libraries"
    )
    add_dependencies(the_sdl2_mixer copy_sdl2_mixer_dll)
else()
    if(EMSCRIPTEN)
        target_compile_options(the_sdl2_mixer INTERFACE --use-port=sdl2_mixer:formats=mp3,ogg)
        target_link_options(the_sdl2_mixer INTERFACE --use-port=sdl2_mixer:formats=mp3,ogg)
    else()
        find_package(SDL2_mixer REQUIRED)
        if (TARGET SDL2_mixer::SDL2_mixer)
            target_link_libraries(the_sdl2_mixer INTERFACE SDL2_mixer::SDL2_mixer)
        else()
            target_include_directories(the_sdl2_mixer SYSTEM INTERFACE ${SDL2_MIXER_INCLUDE_DIRS})
            target_link_libraries(the_sdl2_mixer INTERFACE ${SDL2_MIXER_LIBRARIES})
        endif()
    endif()
endif()
