#include "Mode7.h"
#include <cmath>
//https://www.youtube.com/watch?v=0kVM6dJeWaY




void Mode7::DrawMode7Line(int y) //y doesnt skip
{
    float fSampleDepth = (float)y / (windowsize.y / 2.0f);

    float fStartX = (frustum.Far1.x - frustum.Near1.x) / (fSampleDepth)+frustum.Near1.x;
    float fStartY = (frustum.Far1.y - frustum.Near1.y) / (fSampleDepth)+frustum.Near1.y;
    float fEndX = (frustum.Far2.x - frustum.Near2.x) / (fSampleDepth)+frustum.Near2.x;
    float fEndY = (frustum.Far2.y - frustum.Near2.y) / (fSampleDepth)+frustum.Near2.y;


    for (int x = 0; x < windowsize.x; x++) {

        float fSampleWidth = (float)x / windowsize.x;
        float fSampleX = (fEndX - fStartX) * fSampleWidth + fStartX;
        float fSampleY = (fEndY - fStartY) * fSampleWidth + fStartY;

        int texX = (int)fSampleX;
        int texY = (int)fSampleY;

        Color color = BLANK; // default transparent

        if (texX >= 0 && texX < texMap.width &&
            texY >= 0 && texY < texMap.height) {
            color = GetImageColor(imgMap, texX, texY);
        }

        DrawRectangle((int)position.x + x * blowup_scale,
            (int)position.y + blowup_scale * ((int)windowsize.y / 2 + y),
            blowup_scale, blowup_scale, color);
    }
}

void Mode7::Update()
{
    if (IsKeyDown(KEY_Q)) fNear += 0.1f * fSpeed * GetFrameTime();
    if (IsKeyDown(KEY_A)) fNear -= 0.1f * fSpeed * GetFrameTime();

    if (IsKeyDown(KEY_W)) fFar += 0.1f * fSpeed * GetFrameTime();
    if (IsKeyDown(KEY_S)) fFar -= 0.1f * fSpeed * GetFrameTime();

    if (IsKeyDown(KEY_Z)) fFoVHalf += 0.1f * GetFrameTime();
    if (IsKeyDown(KEY_X)) fFoVHalf -= 0.1f * GetFrameTime();

    if (IsKeyDown(KEY_RIGHT)) {
        fWorldA += 1.0f * GetFrameTime();
        skyOffset += 1.0f * fSpeed * 1.5f * GetFrameTime();
    }
    if (IsKeyDown(KEY_LEFT)) {
        fWorldA -= 1.0f * GetFrameTime();
        skyOffset -= 1.0f * fSpeed * 1.5f * GetFrameTime();
    }

    if (IsKeyDown(KEY_UP)) {
        fWorldX += cosf(fWorldA) * fSpeed * GetFrameTime();
        fWorldY += sinf(fWorldA) * fSpeed * GetFrameTime();
    }

    if (IsKeyDown(KEY_DOWN)) {
        fWorldX -= cosf(fWorldA) * fSpeed * GetFrameTime();
        fWorldY -= sinf(fWorldA) * fSpeed * GetFrameTime();
    }

    frustum.Far1.x = fWorldX + cosf(fWorldA - fFoVHalf) * fFar;
    frustum.Far1.y = fWorldY + sinf(fWorldA - fFoVHalf) * fFar;

    frustum.Near1.x = fWorldX + cosf(fWorldA - fFoVHalf) * fNear;
    frustum.Near1.y = fWorldY + sinf(fWorldA - fFoVHalf) * fNear;

    frustum.Far2.x = fWorldX + cosf(fWorldA + fFoVHalf) * fFar;
    frustum.Far2.y = fWorldY + sinf(fWorldA + fFoVHalf) * fFar;

    frustum.Near2.x = fWorldX + cosf(fWorldA + fFoVHalf) * fNear;
    frustum.Near2.y = fWorldY + sinf(fWorldA + fFoVHalf) * fNear;

    skyOffset = fmod(skyOffset, texSky.width);
}
void Mode7::Draw()
{
    Rectangle skySource = { skyOffset, 0, (float)texSky.width, (float)texSky.height };
    Rectangle skyDest = { position.x, position.y, windowsize.x * blowup_scale , windowsize.y / 2 * blowup_scale };

    DrawTexturePro(texSky, skySource, skyDest, { 0, 0 }, 0.0f, WHITE);
    for (int y = 0; y < windowsize.y / 2; y++) {
        DrawMode7Line(y);
    }
    Mode7::Update();
    
}
void Mode7::unload()
{
    UnloadTexture(texSky);
    UnloadTexture(texMap);
    UnloadImage(imgMap);
}