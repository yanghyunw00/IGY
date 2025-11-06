/**
 * \file GamePlayState.cpp
 * \author Hyunwoo Yang
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "GamePlayState.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RGBA.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Dokev/DKColor.hpp"
#include "Dokev/DKImage.hpp"
#include "Dokev/DKShapes.hpp"
#include "Dokev/DKTexture.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Window.hpp"
#include "OpenGL/GL.hpp"
#include <GL/glew.h>
#include <SDL.h>
#include <cmath>
#include <filesystem>
#include <imgui.h>
#include <iomanip>
#include <numbers>
#include <sstream>

// #include "Human.hpp" 

void GamePlayState::Load()
{
    GL::Enable(GL_STENCIL_TEST); // enable for mouse clicking.
    GL::ClearStencil(0);         // reset. might have leftover shit from other gamestates
    CS200::RenderingAPI::SetClearColor(0x252525FF);
    testTexture   = Dokev::LoadTexture("Assets/images/textures/IsoPeak.png");
    auto perlin   = Dokev::GenImagePerlinNoise(256, 256, 0, 0, 0.01f);
    auto perlin2  = Dokev::GenImagePerlinNoise(256, 256, 256, 256, 0.01f);
    noiseTexture  = Dokev::LoadTextureFromImage(perlin);
    noiseTexture2 = Dokev::LoadTextureFromImage(perlin2);

    map.ResetPerlinTexture();
    map.ResetIsoGridMapArray();
    map.InitializePathFinding();

    camera.SetZoom(0.3f);

    try
    {
        smokeSprite = Dokev::AnimatedSprite("Assets/images/Smoke.spt");
    }
    catch (const std::exception& e)
    {
        printf("Failed to load smoke animation: %s\n", e.what());
    }

    smokeEmitter = std::make_unique<Dokev::ParticleEmitter>("Assets/images/Smoke.spt", 20);
    smokeEmitter->SetMaxLife(1.0); // Particles live for 3 seconds

    m_goManager = std::make_unique<GameObjectManager>();
    m_movementSystem = std::make_unique<MovementSystem>();
    m_renderSystem = std::make_unique<RenderSystem>();
    m_aiSystem = std::make_unique<AISystem>(); 

    // human.reserve(100); 
    // human.push_back(Human(Math::fvec2(5.0f, 5.0f))); 
    // human.push_back(Human(Math::fvec2(78.0f, 13.0f)));; 

    // GameObject
    auto human1 = m_goManager->CreateGameObject();
    human1->AddComponent<TransformComponent>(Math::fvec2(5.0f, 5.0f), Math::fvec2(5.0f, 5.0f));
    human1->AddComponent<PathComponent>();
    human1->AddComponent<AIComponent>();
    human1->AddComponent<StatsComponent>();
    human1->AddComponent<RenderComponent>(Dokev::ColorPalette::RED); 

    auto human2 = m_goManager->CreateGameObject();
    human2->AddComponent<TransformComponent>(Math::fvec2(78.0f, 13.0f), Math::fvec2(78.0f, 13.0f));
    human2->AddComponent<PathComponent>();
    human2->AddComponent<AIComponent>();
    human2->AddComponent<StatsComponent>();
    human2->AddComponent<RenderComponent>(Dokev::ColorPalette::BLUE);

    road = std::make_unique<Road>();
    
}

void GamePlayState::Update()
{
    [[maybe_unused]] const auto& environment = Engine::GetWindowEnvironment();
    updateInput();
    const auto& input = Engine::GetInput();

    smokeSprite.Update(environment.DeltaTime);

    if (smokeEmitter)
    {
        smokeEmitter->Update(environment.DeltaTime);
    }

    // Emit particles when pressing p
    if (input.KeyJustPressed(CS230::Input::Keys::P))
    {
        if (smokeEmitter)
        {
            // Emit particles at camera center (world space)
            Math::vec2 emit_pos = camera.GetPosition();
            Math::vec2 emit_vel(0.0, 0.0);
            Math::vec2 direction(0.0, 200.0);
            smokeEmitter->Emit(5, emit_pos, emit_vel, direction, 3.14159 / 4, 0.5);
            printf("Emitted particles at (%.1f, %.1f)! Active: %d\n", emit_pos.x, emit_pos.y, smokeEmitter->GetActiveCount());
        }
    }
    
    // for (auto& h : human) 
    // { 
    //     h.Update(static_cast<float>(environment.DeltaTime), &map);
    // } 

    // 시스템이 모든 GameObject를 업데이트
    m_aiSystem->Update(m_goManager.get(), static_cast<float>(environment.DeltaTime), &map);
    m_movementSystem->Update(m_goManager.get(), static_cast<float>(environment.DeltaTime), &map);

    // Auto immigration system
    immigrationTimer += static_cast<float>(environment.DeltaTime);
    if (immigrationTimer >= immigrationInterval)
    {
        immigrationTimer = 0.0f;
        // human.push_back(Human::CreateImmigrant(&map));
        CreateImmigrantGameObject(&map); 
    }
    
    m_goManager->Refresh();
}

void GamePlayState::Draw() const
{
    CS200::RenderingAPI::Clear();
    auto& renderer2d = Engine::GetRenderer2D();

    auto windowSize = Engine::GetWindow().GetSize();
    auto ndcMatrix  = CS200::build_ndc_matrix(windowSize);

    auto cameraTransform = camera.GetViewMatrix();

    renderer2d.BeginScene(cameraTransform * ndcMatrix);

    GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    map.IsometricDrawMap(index);

    testTexture.DrawEx(0, 0, 90.0f, 1.0f);

    // Draw smoke animation at position (200, 200)
    Math::TransformationMatrix smokeTransform;
    smokeTransform = Math::TranslationMatrix(Math::vec2(200.0, 200.0));
    // smokeSprite.Draw(smokeTransform);

    // Draw particles
    if (smokeEmitter)
    {
        smokeEmitter->Draw(cameraTransform * ndcMatrix);
    }

    // Draw roads
    int currentView = (index % 4) + 1;
    if (road)
    {
        road->drawRoads(map, currentView);
    }

    // Draw human
    // for (auto& h : human) 
    // { 
    //     h.Draw(currentView, &map); 
    // } 

    // RenderSystem
    m_renderSystem->Draw(m_goManager.get(), currentView, &map);

    renderer2d.EndScene();
}

void GamePlayState::DrawImGui()
{
    if (ImGui::Begin("Controller"))
    {
        // PERFORMANCE Section
        ImGui::Text("PERFORMANCE");
        ImGui::Separator();

        const auto& env = Engine::GetWindowEnvironment();
        ImGui::Text("FPS: %d", env.FPS);
        ImGui::Text("Frame time: %.2f ms", static_cast<float>(env.DeltaTime * 1000.0));

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Text("DISPLAY SETTINGS");
        ImGui::Separator();

        if (ImGui::Checkbox("VSync Enabled", &vsyncEnabled))
        {
            if (vsyncEnabled)
            {
                if (SDL_GL_SetSwapInterval(-1) != 0)
                    SDL_GL_SetSwapInterval(1);       
            }
            else
            {
                SDL_GL_SetSwapInterval(0); 
            }
        }

        ImGui::Spacing();
        ImGui::Spacing();

        // CAMERA CONTROLS Section
        ImGui::Text("CAMERA CONTROLS");
        ImGui::Separator();

        float zoom = camera.GetZoom();
        if (ImGui::SliderFloat("Zoom", &zoom, 0.1f, 1.0f))
        {
            camera.SetZoom(zoom);
        }

        ImGui::Spacing(`);

        if (ImGui::Button("Rotate to Map right"))
        {
            index = (index - 1 + 4) % 4;
        }

        if (ImGui::Button("Roatate Map to Left"))
        {
            index = (index + 1) % 4;
        }

        ImGui::Text("Rotate Index: %d", index);

        ImGui::Spacing();
        ImGui::Spacing();

        // HUMAN CONTROLS Section
        ImGui::Text("HUMAN CONTROLS");
        ImGui::Separator();

        ImGui::Text("Total Humans: %zu", m_goManager->GetGameObjects().size());

        if (ImGui::Button("Spawn Immigrant"))
        {
            // human.push_back(Human::CreateImmigrant(&map));
            CreateImmigrantGameObject(&map); 
        }

        ImGui::SameLine();

        if (ImGui::Button("Make All Humans Explore"))
        {
            // for (auto& h : human) 
            // { 
            //     h.DecideNextAction(&map); 
            // }
            m_aiSystem->MakeAllExplore(m_goManager.get()); 
        }

        ImGui::SliderFloat("Immigration Interval (sec)", &immigrationInterval, 1.0f, 30.0f);
        ImGui::Text("Next immigrant in: %.1f sec", immigrationInterval - immigrationTimer);

        ImGui::Separator();
        // if (!human.empty())
        // {
        //     ImGui::Text("Human moving: %s", human->IsMoving() ? "Yes" : "No");
        //     ImGui::Text("Human debug: %s", human->IsDebugMode() ? "ON" : "OFF");

        //     for (auto& h : human)
        //     {

        //     }
        //     ImGui::Text("Human position: (%.1f, %.1f)", pos.x, pos.y);
        // }

        ImGui::Separator();
        ImGui::Text("Custom Path:");

        static int startX = 5;
        static int startY = 5;
        static int endX   = 50;
        static int endY   = 50;

        ImGui::SliderInt("Start X", &startX, 0, GRID_SIZE_X - 1);
        ImGui::SliderInt("Start Y", &startY, 0, GRID_SIZE_Y - 1);
        ImGui::SliderInt("End X", &endX, 0, GRID_SIZE_X - 1);
        ImGui::SliderInt("End Y", &endY, 0, GRID_SIZE_Y - 1);

        // if (ImGui::Button("Find Path"))
        // {
        //     if (human)
        //     {
        //         map.ClearPath();
        //         map.FindPath(startX, startY, endX, endY);
        //         auto path = map.GetCurrentPath();
        //         if (!path.empty())
        //         {
        //             human->FollowPath(path);
        //         }
        //         else
        //         {
        //             printf("No path found from (%d,%d) to (%d,%d)!\n", startX, startY, endX, endY);
        //         }
        //     }
        // }

        ImGui::SameLine();

        // if (ImGui::Button("Set Start to Human Pos"))
        // {
        //     if (human)
        //     {
        //         auto pos = human->GetActualPosition();
        //         startX   = static_cast<int>(pos.x);
        //         startY   = static_cast<int>(pos.y);
        //     }
        // }

        ImGui::Separator();
        ImGui::Text("Road Building:");

        if (road)
        {
            ImGui::Text("Total roads: %zu", road->getRoadCount());
        }

        static int roadStartX = 10;
        static int roadStartY = 10;
        static int roadEndX   = 30;
        static int roadEndY   = 30;

        ImGui::SliderInt("Road Start X", &roadStartX, 0, GRID_SIZE_X - 1);
        ImGui::SliderInt("Road Start Y", &roadStartY, 0, GRID_SIZE_Y - 1);
        ImGui::SliderInt("Road End X", &roadEndX, 0, GRID_SIZE_X - 1);
        ImGui::SliderInt("Road End Y", &roadEndY, 0, GRID_SIZE_Y - 1);

        if (ImGui::Button("Build Road Path"))
        {
            if (road)
            {
                road->buildRoadPath(map, roadStartX, roadStartY, roadEndX, roadEndY);
                printf("Built road from (%d,%d) to (%d,%d). Total roads: %zu\n", roadStartX, roadStartY, roadEndX, roadEndY, road->getRoadCount());
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear All Roads"))
        {
            if (road)
            {
                road->removeAllRoad();
                printf("All roads cleared\n");
            }
        }

        if (ImGui::Button("Add Single Road"))
        {
            if (road)
            {
                road->addRoad(roadStartX, roadStartY);
                printf("Added road at (%d,%d)\n", roadStartX, roadStartY);
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Remove Road"))
        {
            if (road)
            {
                road->removeRoad(roadStartX, roadStartY);
                printf("Removed road at (%d,%d)\n", roadStartX, roadStartY);
            }
        }
    }
    ImGui::End();
}

void GamePlayState::updateInput()
{
    const auto& input       = Engine::GetInput();
    const auto& environment = Engine::GetWindowEnvironment();


    // Re gen
    if (input.KeyJustPressed(CS230::Input::Keys::Space))
    {
        map.ReGenerateMap();
        map.InitializePathFinding();
        map.ClearPath();
        printf("Map regenerated and pathfinding reinitialized\n");
    }

    // for future road, human
    // int currentView = (index % 4) + 1;


    if (input.KeyDown(CS230::Input::Keys::Q))
        camera.AdjustZoom(0.95f);
    if (input.KeyDown(CS230::Input::Keys::E))
        camera.AdjustZoom(1.05f);

    camera.ClampZoom(0.1f, 5.0f);

    constexpr double camera_speed = 1.0;

    if (input.KeyDown(CS230::Input::Keys::A))
    {
        camera.Move({ camera_speed * environment.DeltaTime, 0 });
    }
    if (input.KeyDown(CS230::Input::Keys::D))
    {
        camera.Move({ -camera_speed * environment.DeltaTime, 0 });
    }
    if (input.KeyDown(CS230::Input::Keys::W))
    {
        camera.Move({ 0, -camera_speed * environment.DeltaTime });
    }
    if (input.KeyDown(CS230::Input::Keys::S))
    {
        camera.Move({ 0, camera_speed * environment.DeltaTime });
    }
    // reset
    if (input.KeyDown(CS230::Input::Keys::R))
    {
        camera.Reset();
    }

    // if (mouse.MouseJustPressed(Dokev::Mouse::Button::Left))
    // {
    //     //int x = stencil_value & 0xFF; int y = (stencil_value >> 8) & 0xFF;
    //     int stencil_value;
    //     glReadPixels(mouse.GetPosition().x,mouse.GetPosition().y, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &stencil_value);
    //     int x = stencil_value & 0xFF; int y = (stencil_value >> 8) & 0xFF;
    //     std::cout << "x : " << x << " y : " << y << std::endl;
    // }

    if (input.KeyJustPressed(CS230::Input::Keys::Z))
    {
        index = (index - 1 + 4) % 4;
    }
    else if (input.KeyJustPressed(CS230::Input::Keys::C))
    {
        index = (index + 1) % 4;
    }
}

void GamePlayState::Unload()
{
    Engine::GetLogger().LogEvent("GamePlayState: Unloaded");
}

gsl::czstring GamePlayState::GetName() const
{
    return "IGY";
}

// Human::FindClosestWalkableTile
Math::fvec2 GamePlayState::FindClosestWalkableTile(GridMap* map, Math::fvec2 startPos) const
{
    if (!map)
    {
        return Math::fvec2(GRID_SIZE_X / 2.0f, GRID_SIZE_Y / 2.0f);
    }

    float closestDist = 999999.0f;
    Math::fvec2 closestTile(GRID_SIZE_X / 2.0f, GRID_SIZE_Y / 2.0f);

    GridCell** viewArray = map->GetViewArray(1);

    for (int x = 0; x < GRID_SIZE_X; x++)
    {
        for (int y = 0; y < 5 && y < GRID_SIZE_Y; y++)
        {
            GridCell* cell = viewArray[y * GRID_SIZE_X + x];
            if (cell->getHeight() >= 75.0)  
            {
                float dx = x - startPos.x;
                float dy = y - startPos.y;
                float dist = dx * dx + dy * dy;

                if (dist < closestDist)
                {
                    closestDist = dist;
                    closestTile = Math::fvec2((float)x, (float)y);
                }
            }
        }

        for (int y = GRID_SIZE_Y - 5; y < GRID_SIZE_Y; y++)
        {
            if (y < 0) continue;
            GridCell* cell = viewArray[y * GRID_SIZE_X + x];
            if (cell->getHeight() >= 75.0)
            {
                float dx = x - startPos.x;
                float dy = y - startPos.y;
                float dist = dx * dx + dy * dy;

                if (dist < closestDist)
                {
                    closestDist = dist;
                    closestTile = Math::fvec2((float)x, (float)y);
                }
            }
        }
    }

    for (int y = 0; y < GRID_SIZE_Y; y++)
    {
        for (int x = 0; x < 5 && x < GRID_SIZE_X; x++)
        {
            GridCell* cell = viewArray[y * GRID_SIZE_X + x];
            if (cell->getHeight() >= 75.0)
            {
                float dx = x - startPos.x;
                float dy = y - startPos.y;
                float dist = dx * dx + dy * dy;

                if (dist < closestDist)
                {
                    closestDist = dist;
                    closestTile = Math::fvec2((float)x, (float)y);
                }
            }
        }

        for (int x = GRID_SIZE_X - 5; x < GRID_SIZE_X; x++)
        {
            if (x < 0) continue;
            GridCell* cell = viewArray[y * GRID_SIZE_X + x];
            if (cell->getHeight() >= 75.0)
            {
                float dx = x - startPos.x;
                float dy = y - startPos.y;
                float dist = dx * dx + dy * dy;

                if (dist < closestDist)
                {
                    closestDist = dist;
                    closestTile = Math::fvec2((float)x, (float)y);
                }
            }
        }
    }
    return closestTile;
}

// Human::CreateImmigrant 
void GamePlayState::CreateImmigrantGameObject(GridMap* map)
{
    int side = rand() % 4;
    float spawnX, spawnY;
    const float offset = 10.0f; 

    switch (side)
    {
        case 0: spawnX = (float)(rand() % GRID_SIZE_X); spawnY = -offset; break;
        case 1: spawnX = GRID_SIZE_X - 1 + offset; spawnY = (float)(rand() % GRID_SIZE_Y); break;
        case 2: spawnX = (float)(rand() % GRID_SIZE_X); spawnY = GRID_SIZE_Y - 1 + offset; break;
        case 3: spawnX = -offset; spawnY = (float)(rand() % GRID_SIZE_Y); break;
        default: spawnX = 0.0f; spawnY = 0.0f; break;
    }

    Math::fvec2 spawnPos(spawnX, spawnY);
    auto immigrant = m_goManager->CreateGameObject();
    
    immigrant->AddComponent<TransformComponent>(spawnPos, spawnPos);
    auto pathComp = immigrant->AddComponent<PathComponent>();
    auto aiComp = immigrant->AddComponent<AIComponent>();
    immigrant->AddComponent<StatsComponent>();
    immigrant->AddComponent<RenderComponent>(Dokev::ColorPalette::GREEN); // immigrant color(temp green)

    if (map)
    {
        Math::fvec2 entryPoint = FindClosestWalkableTile(map, spawnPos);

        map->ClearPath();

        std::vector<Math::fvec2> entryPath;

        float dx = entryPoint.x - spawnX;
        float dy = entryPoint.y - spawnY;
        float totalDist = sqrt(dx * dx + dy * dy);

        int numPoints = (int)totalDist + 1; 
        if (numPoints < 2) numPoints = 2;

        for (int i = 0; i <= numPoints; i++)
        {
            float t = (float)i / (float)numPoints; 
            Math::fvec2 point;
            point.x = spawnX + dx * t;
            point.y = spawnY + dy * t;
            entryPath.push_back(point);
        }

        // Human::FollowPath 
        pathComp->path = entryPath;
        pathComp->currentPathIndex = 1;
        pathComp->startPosition = entryPath[0];
        pathComp->targetGridPos = entryPath[1];
        pathComp->isMoving = true;
        pathComp->moveTimer = 0.0f;

        aiComp->state = HumanState::Walking;
    }
}