#version 300 es

precision mediump float;

uniform sampler2D uPositionTexture;
uniform sampler2D uColorTexture;

uniform mat4 uPreviousViewWorldMat;
uniform mat4 uWorldViewMat;
uniform mat4 uLensProjection;

uniform vec2 uMotionBlurEnabled;
uniform vec2 uParameters;

in vec2 vTexCoord;
layout(location = 0) out vec4 FragColor;

void main() {
  int   size       = int(uParameters.x);
  float separation =     uParameters.y;

  vec2 texSize  = textureSize(uColorTexture, 0).xy;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  FragColor = texture(uColorTexture,    texCoord);
  vec4 position1 = texture(uPositionTexture, texCoord);

  if (size <= 0 || separation <= 0.0 || uMotionBlurEnabled.x != 1.0 || position1.a <= 0.0) { return; }

  vec4 position0 = uWorldViewMat * uPreviousViewWorldMat * position1;

  position0      = uLensProjection * position0;
  position0.xyz /= position0.w;
  position0.xy   = position0.xy * 0.5 + 0.5;

  position1      = uLensProjection * position1;
  position1.xyz /= position1.w;
  position1.xy   = position1.xy * 0.5 + 0.5;

  vec2 direction = position1.xy - position0.xy;

  if (length(direction) <= 0.0) { return; }

  direction.xy *= separation;

  vec2 forward  = texCoord;
  vec2 backward = texCoord;

  float count = 1.0;

  for (int i = 0; i < size; ++i) {
    forward  += direction;
    backward -= direction;

    FragColor +=
      texture
        ( uColorTexture
        , forward
        );
    FragColor +=
      texture
        ( uColorTexture
        , backward
        );

    count += 2.0;
  }

  FragColor /= count;
}
