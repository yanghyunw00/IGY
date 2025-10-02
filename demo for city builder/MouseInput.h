#pragma once

/*
I made trhe mouse input be a spereate classs so I dont need to replicate the code needed for dragging across the map
over multiple sepereate files. it is simply better to have a handy function and class to pull from when needed
*/

#include "Vec2.h"
#include "raylib.h"
class MouseInput
{
private:
    Math::vec2 pan = { 0.f, 0.f };
    Math::vec2 lastMousePos = { 0.f, 0.f };
    bool dragging = false;
    float zoom = 1.0f;

public:
    Math::vec2 GetPan() const { return pan; }
    float GetZoom() const { return zoom; }

    void MouseDrag()
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            Math::vec2 mousePos = { (float)GetMouseX(), (float)GetMouseY() };

            if (!dragging) lastMousePos = mousePos;

            Math::vec2 delta = { mousePos.x - lastMousePos.x, mousePos.y - lastMousePos.y };
            pan += delta;

            lastMousePos = mousePos;
            dragging = true;
        }
        else
        {
            dragging = false;
        }
    }

    void MouseScroll()
    {
        float scroll = GetMouseWheelMove();
        if (scroll != 0.f)
        {
            float oldZoom = zoom;
            zoom += scroll * 0.1f;
            if (zoom < 0.1f) zoom = 0.1f;
            if (zoom > 5.0f) zoom = 5.0f;

            // adjust pan so zoom is centered at mouse
            float mouseX = (float)GetMouseX();
            float mouseY = (float)GetMouseY();
            float zoomFactor = zoom / oldZoom;
            pan = pan * zoomFactor + Math::vec2{ mouseX, mouseY } *(1.f - zoomFactor);
        }
    }
};
