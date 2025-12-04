#version 300 es

precision mediump float;

uniform sampler2D uColorTexture;
in vec2 vTexCoord;
layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = texture(uColorTexture, vTexCoord);
}
