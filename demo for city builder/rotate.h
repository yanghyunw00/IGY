#pragma once
#include <cstdint>

/**
 * @brief rotate point (x,y) around (tx,ty) by 0, 90, 180, 270, or 360 degrees
 * @param x input x coordinate
 * @param y input y coordinate
 * @param tx center x
 * @param ty center y
 * @param angleDeg rotation angle in degrees (must be multiple of 90)
 * @param outX reference to store rotated x
 * @param outY reference to store rotated y
 * @return void
 */

void rotatePoint90(float x, float y,
    float tx, float ty,
    int angleDeg,
    float& outX, float& outY)
{
    // translate to origin
    float dx = x - tx;
    float dy = y - ty;

    int rot = ((angleDeg % 360) + 360) % 360; // normalize

    switch (rot)
    {
    case 0:   // 0°
        outX = tx + dx;
        outY = ty + dy;
        break;
    case 90:  // 90° counterclockwise
        outX = tx - dy;
        outY = ty + dx;
        break;
    case 180: // 180°
        outX = tx - dx;
        outY = ty - dy;
        break;
    case 270: // 270° counterclockwise
        outX = tx + dy;
        outY = ty - dx;
        break;
    case 360: // same as 0°
        outX = tx + dx;
        outY = ty + dy;
        break;
    default:  // unsupported angle
        outX = x;
        outY = y;
        break;
    }
}
