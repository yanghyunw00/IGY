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

uniform float uFrameTime;

void main()
{
    float amount = 0.1;

    float toRadians = 3.14159265 / 180.0;

    float random = fract(10000.0 * sin((gl_FragCoord.x + gl_FragCoord.y * uFrameTime) * toRadians));
    
    vec2 texSize = vec2(textureSize(uColorTexture, 0));
    vec2 texCoord = gl_FragCoord.xy / texSize;

    vec4 color = texture(uColorTexture, texCoord);

    amount *= random;

    color.rgb += amount;

    FragColor = color;
}
