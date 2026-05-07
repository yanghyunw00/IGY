/**
 * \file DKCamera.cpp
 * \brief
 * \author Youngwoo Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKCamera.hpp"
#include "../Core/DKUtility.hpp"
#include <algorithm>
#include <cmath>

namespace Dokev
{
    Camera::Camera(Math::vec2 position, float zoom) : position(position), zoom(zoom)
    {
    }

    Math::TransformationMatrix Camera::GetViewMatrix() const
    {
        return Math::ScaleMatrix({ zoom, zoom }) * Math::TranslationMatrix(position + shakeOffset);
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

    void Camera::AdjustTargetZoom(float factor, float minZoom, float maxZoom)
    {
        targetZoom = std::clamp(targetZoom * factor, minZoom, maxZoom);
    }

    void Camera::ClampZoom(float minZoom, float maxZoom)
    {
        zoom = std::clamp(zoom, minZoom, maxZoom);
    }

    void Camera::ZoomIn()
    {
        if (zoomLevel < 2)
        {
            zoomLevel++;
        }

        switch (zoomLevel)
        {
            case 0: targetZoom = 0.3f; break;
            case 1: targetZoom = 0.8f; break;
            case 2: targetZoom = 1.3f; break;
            default: targetZoom = 1.0f; break;
        }
    }

    void Camera::ZoomOut()
    {
        if (zoomLevel > 0)
        {
            zoomLevel--;
        }

        switch (zoomLevel)
        {
            case 0: targetZoom = 0.3f; break;
            case 1: targetZoom = 0.8f; break;
            case 2: targetZoom = 1.3f; break;
            default: targetZoom = 1.0f; break;
        }
    }

    void Camera::UpdateZoomInterpolation(double dt)
    {
        if (std::abs(zoom - targetZoom) > 0.001f)
        {
            float step = zoomSpeed * static_cast<float>(dt);
            zoom       = zoom + (targetZoom - zoom) * step;

            if (std::abs(zoom - targetZoom) < 0.001f)
            {
                zoom = targetZoom;
            }
        }

    }

    void Camera::StartShake(float amount, float duration)
    {
        shakeAmount   = amount;
        shakeTimeLeft = duration;
    }

    void Camera::UpdateShake(double dt)
    {
        if (shakeTimeLeft <= 0.0f)
        {
            shakeOffset   = { 0.0f, 0.0f };
            shakeTimeLeft = 0.0f;
            return;
        }

        shakeTimeLeft -= static_cast<float>(dt);

        float progress = std::max(0.0f, shakeTimeLeft);
        float decay    = progress * shakeAmount;

        shakeOffset = { GetRandomFloat(-decay, decay), GetRandomFloat(-decay, decay) };
    }

    bool Camera::IsShaking() const
    {
        return shakeTimeLeft > 0.0f;
    }

    void Camera::Reset()
    {
        position      = { 0.0, 0.0 };
        zoom          = 0.8f;
        targetZoom    = 0.8f;
        zoomLevel     = 1;
        shakeOffset   = { 0.0f, 0.0f };
        shakeTimeLeft = 0.0f;
    }
}
