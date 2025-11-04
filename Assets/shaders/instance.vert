#version 300 es
/**
 * \file instanced.vert
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
layout(location = 0) in vec2 aLocalPosition;
layout(location = 1) in vec3 aTransformRow0;
layout(location = 2) in vec3 aTransformRow1;
layout(location = 3) in vec3 aTransformRow2;
layout(location = 4) in vec2 aTexCoord0;  // Bottom left
layout(location = 5) in vec2 aTexCoord1;  // Bottom right
layout(location = 6) in vec2 aTexCoord2;  // Top right
layout(location = 7) in vec2 aTexCoord3;  // Top left
layout(location = 8) in vec4 aTint;
layout(location = 9) in int aTextureIndex;

// SDF attributes
layout(location = 10) in int aShapeType;      // -1=texture, 0=circle, 1=rectangle
layout(location = 11) in vec2 aWorldSize; 
layout(location = 12) in float aLineWidth;    
layout(location = 13) in vec4 aLineColor;   

out vec2 vTexCoord;
out vec2 vLocalPosition;
flat out vec4 vTint;
flat out int vTextureIndex;

flat out int vShapeType;
flat out vec2 vWorldSize;
flat out float vLineWidth;
flat out vec4 vLineColor;

uniform mat3 uToNDC;

void main()
{
    mat3 model = mat3(aTransformRow0, aTransformRow1, aTransformRow2);

    vec3 world_point = model * vec3(aLocalPosition, 1.0);
    vec3 ndc_point = uToNDC * world_point;

    gl_Position = vec4(ndc_point.xy, 0.0, 1.0);

    if (gl_VertexID % 4 == 0)
        vTexCoord = aTexCoord0;  // Bottom left
    else if (gl_VertexID % 4 == 1)
        vTexCoord = aTexCoord1;  // Bottom right
    else if (gl_VertexID % 4 == 2)
        vTexCoord = aTexCoord2;  // Top right
    else
        vTexCoord = aTexCoord3;  // Top left

    vTint = aTint;
    vTextureIndex = aTextureIndex;

    vLocalPosition = aLocalPosition;
    vShapeType = aShapeType;
    vWorldSize = aWorldSize;
    vLineWidth = aLineWidth;
    vLineColor = aLineColor;
}
