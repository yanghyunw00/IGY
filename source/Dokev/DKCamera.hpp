/**
 * \file DKCamera.hpp
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "Engine/Matrix.hpp"
#include "Engine/Vec2.hpp"

namespace Dokev
{

    class Camera
    {
    public:
        Camera() = default;
        Camera(Math::vec2 position, float zoom);

        Math::TransformationMatrix GetViewMatrix() const;

        void Move(Math::vec2 offset);

        void SetPosition(Math::vec2 position);

        Math::vec2 GetPosition() const;

        void SetZoom(float zoom);

        float GetZoom() const;

        void AdjustZoom(float factor);

        void ClampZoom(float minZoom, float maxZoom);

        void Reset();

    private:
        Math::vec2 position{ 0.0, 0.0 };
        float      zoom{ 1.0f };
    };
}