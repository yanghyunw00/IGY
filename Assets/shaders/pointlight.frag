#version 300 es
precision mediump float;

/**
 * \file pointlight.frag
 * \author Dongyun Lee
 * \date 2026 Spring
 */

const int MAX_LIGHTS = 64;

uniform int   uLightCount;
uniform vec2  uLightPos[MAX_LIGHTS];
uniform float uLightRadius[MAX_LIGHTS];
uniform float uNightIntensity;

layout(location = 0) out vec4 FragColor;

void main()
{
    vec2 fragPos = gl_FragCoord.xy;
    vec3 color   = vec3(0.0);

    for (int i = 0; i < uLightCount; i++)
    {
        float dist  = length(fragPos - uLightPos[i]);
        float atten = max(0.0, 1.0 - dist / uLightRadius[i]);
        atten       = atten * atten;
        color      += vec3(1.0, 0.82, 0.35) * atten;
    }

    color    *= uNightIntensity;
    FragColor = vec4(color, 1.0);
}
