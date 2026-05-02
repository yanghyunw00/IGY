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
    Engine/AudioManager.hpp Engine/AudioManager.cpp
    Engine/FontManager.hpp Engine/FontManager.cpp


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

    # Dokev Framework - Core
    Dokev/Core/DKMath.hpp Dokev/Core/DKMath.cpp
    Dokev/Core/DKUtility.hpp Dokev/Core/DKUtility.cpp
    Dokev/Core/DKColor.hpp Dokev/Core/DKColor.cpp
    Dokev/Core/DKMetaProgramming.hpp
    Dokev/Core/DKSystemInfo.hpp Dokev/Core/DKSystemInfo.cpp

    # Dokev Framework - Managers
    Dokev/Managers/DKGameObjectManager.hpp Dokev/Managers/DKGameObjectManager.cpp
    Dokev/Managers/DKComponentManager.hpp Dokev/Managers/DKComponentManager.cpp
    Dokev/Managers/DKMapManager.hpp Dokev/Managers/DKMapManager.cpp
    Dokev/Managers/DKUIManager.hpp Dokev/Managers/DKUIManager.cpp
    Dokev/Managers/DKTimeManager.hpp Dokev/Managers/DKTimeManager.cpp
    Dokev/Managers/DKTileManager.hpp Dokev/Managers/DKTileManager.cpp
    Dokev/Managers/DKJobsystemManager.hpp

    # Dokev Framework - GameObject
    Dokev/GameObject/DKGameObject.hpp
    Dokev/GameObject/DKGameObjectTypes.hpp
    Dokev/GameObject/DKComponent.hpp
    Dokev/GameObject/DKJobsystem.hpp Dokev/GameObject/DKJobsystem.cpp

    # Dokev Framework - Graphics
    Dokev/Graphics/DKCamera.hpp Dokev/Graphics/DKCamera.cpp
    Dokev/Graphics/DKTexture.hpp Dokev/Graphics/DKTexture.cpp
    Dokev/Graphics/DKImage.hpp Dokev/Graphics/DKImage.cpp
    Dokev/Graphics/DKSpriteSheet.hpp Dokev/Graphics/DKSpriteSheet.cpp
    Dokev/Graphics/DKAnimationSprite.hpp Dokev/Graphics/DKAnimationSprite.cpp
    Dokev/Graphics/DKAnimation.hpp Dokev/Graphics/DKAnimation.cpp
    Dokev/Graphics/DKShapes.hpp Dokev/Graphics/DKShapes.cpp
    Dokev/Graphics/DKRenderState.hpp
    Dokev/Graphics/DKParticleEmitter.hpp Dokev/Graphics/DKParticleEmitter.cpp
    Dokev/Graphics/DKParticles.hpp

    # Dokev Framework - Input
    Dokev/Input/DKMouseInput.hpp Dokev/Input/DKMouseInput.cpp

    # Dokev Framework - World
    Dokev/World/DKGrid.hpp Dokev/World/DKGrid.cpp
    Dokev/World/DKCollision.hpp Dokev/World/DKCollision.cpp

    # Dokev Framework - State
    Dokev/State/DKGameState.hpp

    # Game - States
    Game/States/GamePlayState.hpp Game/States/GamePlayState.cpp
    Game/States/MainMenu.hpp Game/States/MainMenu.cpp
    Game/States/Splash.hpp Game/States/Splash.cpp
    Game/States/Setting.hpp Game/States/Setting.cpp
    Game/States/GameCredit.hpp Game/States/GameCredit.cpp
    Game/States/DEMO.hpp Game/States/DEMO.cpp
    Game/States/PlainsDemo.hpp Game/States/PlainsDemo.cpp
    Game/States/WaterDemo.hpp Game/States/WaterDemo.cpp
    Game/States/MountainDemo.hpp Game/States/MountainDemo.cpp

    # Game - Entities
    Game/Entities/Human.hpp Game/Entities/Human.cpp
    Game/Entities/Building.hpp Game/Entities/Building.cpp
    Game/Entities/ResidentBuilding.hpp Game/Entities/ResidentBuilding.cpp
    Game/Entities/WorkBuilding.hpp Game/Entities/WorkBuilding.cpp
    Game/Entities/Road.hpp Game/Entities/Road.cpp

    # Game - Resources
    Game/Resources/Resource.hpp Game/Resources/Resource.cpp
    Game/Resources/Wood.hpp Game/Resources/Wood.cpp
    Game/Resources/Stone.hpp Game/Resources/Stone.cpp
    Game/Resources/Steel.hpp Game/Resources/Steel.cpp

    # Game - Managers
    Game/Managers/AchievementManager.hpp Game/Managers/AchievementManager.cpp
    Game/Managers/EventManager.hpp Game/Managers/EventManager.cpp
    Game/Managers/ResourceManager.hpp
    Game/Managers/BuildingCostManager.hpp Game/Managers/BuildingCostManager.cpp
    Game/Managers/HappinessManager.hpp
    Game/Managers/DKCityMetrics.hpp Game/Managers/DKCityMetrics.cpp
    Game/Managers/DKImmigrationManager.hpp Game/Managers/DKImmigrationManager.cpp
    Game/Managers/FloatingTextManager.hpp Game/Managers/FloatingTextManager.cpp
    Game/Managers/WorkManager.hpp Game/Managers/WorkManager.cpp
    Game/Managers/NPCNameManager.hpp Game/Managers/NPCNameManager.cpp

    # Game - World
    Game/World/GridMap.hpp Game/World/GridMap.cpp
    Game/World/GridCell.hpp
    Game/World/CustomCell.hpp Game/World/CustomCell.cpp
    Game/World/pathFinding.hpp Game/World/pathFinding.cpp
    Game/World/DisasterEvent.hpp
    Game/World/Drought.hpp Game/World/Drought.cpp
    Game/World/Meteor.hpp Game/World/Meteor.cpp
    Game/World/Fire.hpp Game/World/Fire.cpp



    # Game - UI
    Game/UI/DKUserInterfaceEditor.hpp Game/UI/DKUserInterfaceEditor.cpp
    Game/UI/DKTutorialManager.hpp Game/UI/DKTutorialManager.cpp
    Game/UI/DKTutorialStep.hpp Game/UI/DKTutorialStep.cpp

    # Game - Input
    Game/Input/DKMouseSelection.hpp Game/Input/DKMouseSelection.cpp

    # Game - Editor
    Game/Editor/DKGameWorldEditor.hpp Game/Editor/DKGameWorldEditor.cpp

    # Game - Common
    Game/Common/BuildingType.hpp
    Game/Common/Component.hpp


   # external/stb_image.h
 
    main.cpp
)

add_executable(cs200_fun ${SOURCE_CODE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_CODE})

target_link_libraries(cs200_fun PRIVATE project_options dependencies)
target_include_directories(cs200_fun PRIVATE .)

# Precompiled header
target_precompile_headers(cs200_fun PRIVATE pch.hpp)

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
