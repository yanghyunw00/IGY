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

class GridMap;
class Human;
class Road;

namespace Dokev
{
    class Camera;

    class GameWorldEditor
    {
    public:
        GameWorldEditor() = default;

        void Initialize(GridMap* mapPtr, Camera* cameraPtr, std::vector<Human>* humanPtr, Road* roadPtr,
                       int* indexPtr, float* immigrationTimerPtr, float* immigrationIntervalPtr);

        void UpdateInput();
        void DrawImGui();

    private:
        GridMap* map = nullptr;
        Camera* camera = nullptr;
        std::vector<Human>* humans = nullptr;
        Road* road = nullptr;
        int* rotationIndex = nullptr;
        float* immigrationTimer = nullptr;
        float* immigrationInterval = nullptr;

        bool vsyncEnabled = true;
    };
}