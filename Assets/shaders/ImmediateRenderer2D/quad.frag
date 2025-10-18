#version 300 es
precision mediump float;
precision mediump sampler2D;

/**
 * \file
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec4 uTint;

void main()
{
    vec4 texColor = texture(uTexture, vTexCoord) * uTint;

    if (texColor.a == 0.0)
    {
        discard;
    }
    
    FragColor = texColor;
}