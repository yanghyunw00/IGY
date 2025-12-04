#version 300 es

/**
 * \file
 * \author Junseok Lee
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec4 aTint;

out vec2 vTexCoord;
flat out vec4 vTint;

uniform mat3 uToNDC;

void main()
{
    vec3 ndc_pos = uToNDC * vec3(aPosition, 1.0);
    gl_Position  = vec4(ndc_pos.xy, 0.0, 1.0);

    vTexCoord = aTexCoord;
    vTint     = aTint;
}
