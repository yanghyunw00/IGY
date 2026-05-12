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
class ResourceManager;
class EventManager;

namespace Dokev
{
    class Camera;
    class GameObjectManager;
    class CityMetrics;

    class GameWorldEditor
    {
    public:
        GameWorldEditor() = default;

        void Initialize(
            MapManager* mapManagerPtr, Camera* cameraPtr, int* indexPtr, float* immigrationTimerPtr, float* immigrationIntervalPtr, GameObjectManager* gameObjectManager,
            ResourceManager* resourceManagerPtr, CityMetrics* cityMetricsPtr = nullptr, EventManager* eventManagerPtr = nullptr);

        void UpdateInput(double dt);
        void DrawImGui();

        bool IsSaveRequested() const
        {
            return saveGameRequested;
        }

        bool IsLoadRequested() const
        {
            return loadGameRequested;
        }

        void ClearSaveRequest()
        {
            saveGameRequested = false;
        }

        void ClearLoadRequest()
        {
            loadGameRequested = false;
        }

    private:
        MapManager*        mapManager          = nullptr;
        Camera*            camera              = nullptr;
        int*               rotationIndex       = nullptr;
        float*             immigrationTimer    = nullptr;
        float*             immigrationInterval = nullptr;
        GameObjectManager* goManager           = nullptr;
        ResourceManager*   resourceManager     = nullptr;
        CityMetrics*       cityMetrics         = nullptr;
        EventManager*      eventManager        = nullptr;

        bool vsyncEnabled         = true;
        bool saveGameRequested    = false;
        bool loadGameRequested    = false;
        bool disasterDebugDraw    = false;
    };
}
