#version 300 es

/**
 * \file
 * \author Junseok Lee
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
// the shader takes in world coordinates that are computed cpu side
layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec4 aTint;

out vec2 vTexCoord; // interpolated texture coords
flat out vec4 vTint; // flat tells that we dont need to do any interpolation for this

uniform mat3 uToNDC;
uniform float uDepth;

void main()
{
    vec3 ndc_pos = uToNDC * vec3(aVertexPosition, 1.0);
    gl_Position = vec4(ndc_pos.xy, uDepth, 1.0);
    
    vTexCoord = aTexCoord;
    vTint = aTint;
}
    