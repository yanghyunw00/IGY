#version 300 es

precision mediump float;

/**
 * \file
 * \author Dongyun Lee
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

uniform sampler2D uColorTexture;
in vec2           vTexCoord;
layout(location = 0) out vec4 FragColor;

uniform int uPixelSize;

void main()
{
    vec2 texSize = vec2(textureSize(uColorTexture, 0));

    float px = max(float(uPixelSize), 1.0);

    // Sample at the center of the current pixel block to create a pixelation effect.
    vec2 blockCoord = floor(gl_FragCoord.xy / px) * px + px * 0.5;
    FragColor       = texture(uColorTexture, blockCoord / texSize);
}
