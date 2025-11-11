/**
 * \file DKGameWorldEditor.hpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include <memory>
#include <vector>

class MapManager;

namespace Dokev
{
    class Camera;
    class GameObjectManager;

    class GameWorldEditor
    {
    public:
        GameWorldEditor() = default;

        void Initialize(MapManager* mapManagerPtr, Camera* cameraPtr,
                       int* indexPtr, float* immigrationTimerPtr, float* immigrationIntervalPtr,
                       GameObjectManager* gameObjectManager);

        void UpdateInput();
        void DrawImGui();

    private:
        MapManager* mapManager = nullptr;
        Camera* camera = nullptr;
        int* rotationIndex = nullptr;
        float* immigrationTimer = nullptr;
        float* immigrationInterval = nullptr;
        GameObjectManager* goManager = nullptr;

        bool vsyncEnabled = true;
    };
}