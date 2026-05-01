#version 300 es

precision mediump float;

/**
 * \file rain.frag
 * \author Dongyun Lee
 * \date 2026 Spring
 */

uniform float uTime;
uniform float uRainIntensity;
uniform vec2  uResolution;

in  vec2 vTexCoord;
layout(location = 0) out vec4 FragColor;

float hash(vec2 p)
{
    p = fract(p * vec2(127.1, 311.7));
    p += dot(p, p + 19.19);
    return fract(p.x * p.y);
}

void main()
{
    vec2 uv = vTexCoord * uResolution;

    float angle = -0.13;  // slight wind lean

    float rain = 0.0;

    for (int layer = 0; layer < 3; layer++)
    {
        float fi = float(layer);

        // Each layer has different scale/speed for depth feel
        float scale      = 1.0 + fi * 0.8;
        float speed      = (380.0 + uRainIntensity * 180.0) / scale;
        float layerAlpha = 1.0 - fi * 0.28;

        vec2 sheared = uv / scale;
        sheared.x   += sheared.y * angle;

        // Wide cells so each has at most one streak — reduces density
        // More intense rain = slightly narrower cells (more streaks)
        float cellW = 32.0 - uRainIntensity * 10.0;
        float cellH = 80.0 + fi * 30.0;

        float scrolled = sheared.y + uTime * speed;

        float cellX = floor(sheared.x / cellW);
        float cellY = floor(scrolled / cellH);

        // Per-cell random seed — changes each cycle so streaks aren't always in same spot
        float seed = hash(vec2(cellX + fi * 17.3, cellY + fi * 41.7));

        // Only a fraction of cells have a streak; heavier rain = more cells active
        float threshold = 0.55 - uRainIntensity * 0.30;
        if (seed > threshold)
            continue;

        // Random x offset within the cell (breaks the grid look)
        float xJitter    = hash(vec2(cellX * 5.1 + fi, cellY * 3.7)) * cellW;
        float streakCX   = cellX * cellW + xJitter;
        float xDist      = abs(sheared.x - streakCX);

        // Thin streak — 0.8 to 1.6 px wide, varies per streak
        float streakW    = 0.8 + hash(vec2(cellX, cellY + fi * 7.0)) * 0.8;
        float xMask      = step(xDist, streakW);

        // Streak length varies: 15–55% of cell height
        float lenFrac    = 0.15 + hash(vec2(cellX * 2.9, cellY + fi)) * 0.40;
        float streakLen  = cellH * lenFrac;

        // Y position within the cell — streak head falls from top, tail fades
        float localY     = mod(scrolled, cellH);
        float yFade      = clamp(1.0 - localY / streakLen, 0.0, 1.0);

        // Gap after streak: silence for the bottom portion of the cell
        float streakOn   = step(localY, streakLen);

        // Per-streak brightness variation
        float brightness = 0.45 + seed * 0.45;

        rain += xMask * yFade * streakOn * layerAlpha * brightness;
    }

    float alpha = clamp(rain * uRainIntensity * 0.55, 0.0, 0.42);
    vec3 rainColor = vec3(0.74, 0.84, 1.0);
    FragColor = vec4(rainColor, alpha);
}
