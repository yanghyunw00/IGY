# author Rudy Castan
# date 2025 Fall
# CS200 Computer Graphics I
# copyright DigiPen Institute of Technology

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(SOURCE_CODE 

    CS200/Image.hpp CS200/Image.cpp
    CS200/ImGuiHelper.hpp CS200/ImGuiHelper.cpp
    CS200/ImmediateRenderer2D.hpp CS200/ImmediateRenderer2D.cpp
    CS200/InstancedRenderer2D.hpp CS200/InstancedRenderer2D.cpp
    CS200/BatchRenderer2D.hpp CS200/BatchRenderer2D.cpp
    CS200/IRenderer2D.hpp
    CS200/NDC.hpp
    CS200/Renderer2DUtils.hpp CS200/Renderer2DUtils.cpp
    CS200/RenderingAPI.hpp CS200/RenderingAPI.cpp
    CS200/RGBA.hpp
    CS200/OffscreenFramebuffer.hpp CS200/OffscreenFramebuffer.cpp
    CS200/PostProcessingPipeline.hpp CS200/PostProcessingPipeline.cpp

    Engine/Engine.hpp Engine/Engine.cpp
    Engine/Error.hpp
    Engine/Font.hpp Engine/Font.cpp
    Engine/FPS.hpp
    Engine/GameState.hpp
    Engine/GameStateManager.hpp Engine/GameStateManager.cpp
    Engine/Input.hpp Engine/Input.cpp
    Engine/Logger.hpp Engine/Logger.cpp
    Engine/Matrix.hpp Engine/Matrix.cpp
    Engine/Path.hpp Engine/Path.cpp
    Engine/Random.hpp Engine/Random.cpp
    Engine/Rect.hpp
    Engine/Texture.hpp Engine/Texture.cpp
    Engine/TextureManager.hpp Engine/TextureManager.cpp
    Engine/Timer.hpp
    Engine/Vec2.hpp Engine/Vec2.cpp
    Engine/Window.hpp Engine/Window.cpp

    OpenGL/Buffer.hpp OpenGL/Buffer.cpp
    OpenGL/Environment.hpp
    OpenGL/Framebuffer.hpp OpenGL/Framebuffer.cpp
    OpenGL/GL.cpp OpenGL/GL.hpp
    OpenGL/GLConstants.hpp
    OpenGL/GLTypes.hpp
    OpenGL/Handle.hpp
    OpenGL/Shader.cpp OpenGL/Shader.hpp
    OpenGL/Texture.hpp OpenGL/Texture.cpp
    OpenGL/VertexArray.cpp OpenGL/VertexArray.hpp

    Dokev/DKAnimation.hpp Dokev/DKAnimation.cpp
    Dokev/DKAnimationSprite.hpp Dokev/DKAnimationSprite.cpp
    Dokev/DKCamera.hpp Dokev/DKCamera.cpp
    Dokev/DKCityMetrics.hpp Dokev/DKCityMetrics.cpp
    Dokev/DKCollision.hpp Dokev/DKCollision.cpp
    Dokev/DKColor.hpp Dokev/DKColor.cpp
    Dokev/DKComponent.hpp
    Dokev/DKComponentManager.hpp Dokev/DKComponentManager.cpp
    Dokev/DKGameObject.hpp
    Dokev/DKGameObjectManager.hpp Dokev/DKGameObjectManager.cpp
    Dokev/DKGameObjectTypes.hpp
    Dokev/DKGameState.hpp
    Dokev/DKGameWorldEditor.hpp Dokev/DKGameWorldEditor.cpp
    Dokev/DKGrid.hpp Dokev/DKGrid.cpp
    Dokev/DKImage.hpp Dokev/DKImage.cpp
    Dokev/DKMath.hpp Dokev/DKMath.cpp
    Dokev/DKMouseInput.hpp Dokev/DKMouseInput.cpp
    Dokev/DKMapManager.hpp Dokev/DKMapManager.cpp
    Dokev/DKParticleEmitter.hpp Dokev/DKParticleEmitter.cpp
    Dokev/DKParticles.hpp
    Dokev/DKShapes.hpp Dokev/DKShapes.cpp
    Dokev/DKSpriteSheet.hpp Dokev/DKSpriteSheet.cpp
    Dokev/DKSystemInfo.hpp Dokev/DKSystemInfo.cpp
    Dokev/DKTexture.hpp Dokev/DKTexture.cpp
    Dokev/DKTileManager.hpp Dokev/DKTileManager.cpp
    Dokev/DKTimeManager.hpp Dokev/DKTimeManager.cpp
    Dokev/DKUIManager.hpp Dokev/DKUIManager.cpp
    Dokev/DKUtility.hpp Dokev/DKUtility.cpp
    Dokev/DKTutorialManager.hpp Dokev/DKTutorialManager.cpp
    Dokev/DkTutorialStep.hpp Dokev/DkTutorialStep.cpp  
    Dokev/DKJobsystem.hpp Dokev/DKJobsystem.cpp
    Dokev/DKJobsystemManager.hpp
    Dokev/DKMouseSelection.hpp Dokev/DKMouseSelection.cpp
    Dokev/DKMetaProgramming.hpp
    Dokev/DKUserInterfaceEditor.hpp Dokev/DKUserInterfaceEditor.cpp
    Dokev/DKImmigrationSystem.hpp Dokev/DKImmigrationSystem.cpp



    Game/BuildingType.hpp
    Game/BuildingCostManager.hpp Game/BuildingCostManager.cpp
    Game/GamePlayState.hpp Game/GamePlayState.cpp
    Game/DEMO.hpp Game/DEMO.cpp
    Game/PlainsDemo.hpp Game/PlainsDemo.cpp
    Game/WaterDemo.hpp Game/WaterDemo.cpp
    Game/MountainDemo.hpp Game/MountainDemo.cpp
    Game/GridCell.hpp
    Game/CustomCell.hpp Game/CustomCell.cpp
    Game/GridMap.hpp Game/GridMap.cpp
    Game/HappinessSystem.hpp
    Game/Human.hpp Game/Human.cpp
    Game/pathFinding.hpp Game/pathFinding.cpp
    Game/Road.hpp Game/Road.cpp
    Game/Resource.hpp Game/Resource.cpp
    Game/Wood.hpp Game/Wood.cpp
    Game/Stone.hpp Game/Stone.cpp
    Game/Steel.hpp Game/Steel.cpp
    Game/ResourceManager.hpp


    Game/ResidentBuilding.hpp Game/ResidentBuilding.cpp
    Game/Building.hpp Game/Building.cpp
    Game/WorkBuilding.hpp Game/WorkBuilding.cpp 
    Game/MainMenu.hpp Game/MainMenu.cpp

   # external/stb_image_write.h
   # external/stb_image.h
   # external/stb_perlin.h



    main.cpp
)

add_executable(cs200_fun ${SOURCE_CODE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_CODE})

target_link_libraries(cs200_fun PRIVATE project_options dependencies)
target_include_directories(cs200_fun PRIVATE .)

# Copy Assets folder to build directory automatically
add_custom_command(
    TARGET cs200_fun POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Assets
    $<TARGET_FILE_DIR:cs200_fun>/Assets
    COMMENT "Copying Assets folder to build directory"
)

# Check the IS_DEVELOPER_VERSION cache variable
# This is set by the cmake configure preset
if (IS_DEVELOPER_VERSION)
    target_compile_definitions(cs200_fun PRIVATE DEVELOPER_VERSION)
endif()

if(EMSCRIPTEN)

    # https://emscripten.org/docs/tools_reference/settings_reference.html
    # ASSERTIONS=1                  - we want asserts to work
    # WASM=1                        - we want web assembly generated rather than just javascript
    # DISABLE_EXCEPTION_CATCHING=0  - we are using exceptions so let's not disable them
    # ALLOW_MEMORY_GROWTH=1         - we don't know exactly how much heap space we'll need so allow it to grow
    # EXIT_RUNTIME=1                - have exiting actually stop the program
    # SINGLE_FILE=1                 - generate everything into one html file
    # --embed-file                  - https://emscripten.org/docs/tools_reference/emcc.html#emcc-embed-file
    # --use-preload-cache           - help with faster reloads : https://emscripten.org/docs/compiling/Deploying-Pages.html#providing-a-quick-second-time-load
    # -lembind                      - to call c++ from javascript https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html
    # --shell-file                  - to customize the webpage https://emscripten.org/docs/compiling/Deploying-Pages.html#build-files-and-custom-shell
    target_link_options(cs200_fun PRIVATE 
    -sASSERTIONS=$<$<BOOL:${IS_DEVELOPER_VERSION}>:1>$<$<NOT:$<BOOL:${IS_DEVELOPER_VERSION}>>:0>
    -sWASM=1 
    -sDISABLE_EXCEPTION_CATCHING=0 
    -sALLOW_MEMORY_GROWTH=1 
    -sEXIT_RUNTIME=1 
    -sSINGLE_FILE=1 
    --embed-file ${CMAKE_SOURCE_DIR}/Assets@/Assets
    --use-preload-cache
    -lembind
    --shell-file ${CMAKE_SOURCE_DIR}/app_resources/web/index_shell.html
    )

    set_target_properties(cs200_fun PROPERTIES SUFFIX ".html")
elseif(WIN32)

    target_link_options(cs200_fun PRIVATE 
        $<IF:$<BOOL:${IS_DEVELOPER_VERSION}>,/SUBSYSTEM:CONSOLE,/SUBSYSTEM:WINDOWS>
        /ENTRY:mainCRTStartup)

    set(ICON_RC ${CMAKE_CURRENT_BINARY_DIR}/app.rc)
    if(NOT EXISTS ${ICON_RC})
        set(ICON_FILE ${CMAKE_SOURCE_DIR}/app_resources/windows/icon.ico)
        file(WRITE ${ICON_RC} "IDR_MAINFRAME ICON \"${ICON_FILE}\"\nIDI_ICON1 ICON DISCARDABLE \"${ICON_FILE}\"")
    endif()

    target_sources(cs200_fun PRIVATE ${ICON_RC})

endif()
