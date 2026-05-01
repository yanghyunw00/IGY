#version 300 es

precision mediump float;

/**
 * \file gaussian-blur.frag
 * \author Dongyun Lee
 * \date 2026 0501
 */

uniform sampler2D uColorTexture;
uniform vec2 uResolution;
uniform bool uHorizontal;

in vec2 vTexCoord;
layout(location = 0) out vec4 FragColor;

void main()
{
    vec2 texel = 1.0 / uResolution;
    vec2 dir = uHorizontal ? vec2(texel.x, 0.0) : vec2(0.0, texel.y);

    // 9-tap separable Gaussian, samples spread 3 texels apart for a wide soft blur
    const float w0 = 0.227027;
    const float w1 = 0.194595;
    const float w2 = 0.121622;
    const float w3 = 0.054054;
    const float w4 = 0.016216;

    vec4 result = texture(uColorTexture, vTexCoord) * w0;
    result += texture(uColorTexture, vTexCoord + dir * 3.0) * w1;
    result += texture(uColorTexture, vTexCoord - dir * 3.0) * w1;
    result += texture(uColorTexture, vTexCoord + dir * 6.0) * w2;
    result += texture(uColorTexture, vTexCoord - dir * 6.0) * w2;
    result += texture(uColorTexture, vTexCoord + dir * 9.0) * w3;
    result += texture(uColorTexture, vTexCoord - dir * 9.0) * w3;
    result += texture(uColorTexture, vTexCoord + dir * 12.0) * w4;
    result += texture(uColorTexture, vTexCoord - dir * 12.0) * w4;

    FragColor = result;
}
