#version 300 es

/**
 * \file
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

layout(std140) uniform Camera
{
    mat3 uViewProjection;
};

uniform mat3 uModel;
uniform mat3 uTexCoordTransform;
uniform float uDepth;

void main()
{
    vec3 ndc_pos = uViewProjection * uModel * vec3(aVertexPosition, 1.0);
    gl_Position = vec4(ndc_pos.xy, uDepth, 1.0);
    
    vec3 tex_coords = uTexCoordTransform * vec3(aTexCoord, 1.0);
    vTexCoord = tex_coords.st;
}