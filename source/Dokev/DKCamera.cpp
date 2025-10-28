/**
 * \file DKCamera.cpp
 * \brief 
 * \author Youngwoo Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKCamera.hpp"
#include <algorithm>

namespace Dokev
{
    Camera::Camera(Math::vec2 position, float zoom)
        : position(position), zoom(zoom)
    {
    }

    Math::TransformationMatrix Camera::GetViewMatrix() const
    {
        return Math::ScaleMatrix({ zoom, zoom }) * Math::TranslationMatrix(position);
    }

    void Camera::Move(Math::vec2 offset)
    {
        position += offset;
    }

    void Camera::SetPosition(Math::vec2 newPosition)
    {
        position = newPosition;
    }

    Math::vec2 Camera::GetPosition() const
    {
        return position;
    }

    void Camera::SetZoom(float newZoom)
    {
        zoom = newZoom;
    }

    float Camera::GetZoom() const
    {
        return zoom;
    }

    void Camera::AdjustZoom(float factor)
    {
        // should translate 
        zoom *= factor;
    }

    void Camera::ClampZoom(float minZoom, float maxZoom)
    {
        zoom = std::clamp(zoom, minZoom, maxZoom);
    }

    void Camera::Reset()
    {
        position = { 0.0, 0.0 };
        zoom = 1.0f;
    }
}
