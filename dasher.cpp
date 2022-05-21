#include "raylib.h"

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isInAirCheck(AnimData data, int windowHeight)
{
    return data.pos.y <= windowHeight - data.rec.height;
}

AnimData UpdateAnimData(AnimData data, float deltaT, int maxFrame)
{
    data.runningTime = data.runningTime + deltaT;

    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }
    return data;
}

int main(void)
{
    // window dimensions
    const int windowWidth{512};
    const int windowHeight{380};

    // initialize the window
    InitWindow(windowWidth, windowHeight, "Dapper Dasher!");
    SetTargetFPS(144);

    // background far
    Texture2D backgroundFar = LoadTexture("textures/far-buildings.png");
    float bgFarX{};

    // background mid
    Texture2D backgroundMid = LoadTexture("textures/back-buildings.png");
    float bgMidX{};

    // background close
    Texture2D backgroundClose = LoadTexture("textures/foreground.png");
    float bgCloseX{};

    // nebula variables
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    
    // acceleration due to gravity (pixels/s)/s
    const int gravity{1000};

    // AnimData for nebula array
    int sizeOfNebulaArray = 10;
    AnimData nebulaArray[sizeOfNebulaArray]{};

    for (int i = 0; i < sizeOfNebulaArray; i++)
    {
        nebulaArray[i].rec = {0.0f, 0.0f, nebula.width / 8.0f, nebula.height / 8.0f};
        nebulaArray[i].pos = {windowWidth + i * 500.0f + GetRandomValue(0,300), windowHeight - nebula.height / 8.0f};
        nebulaArray[i].frame = 0;
        nebulaArray[i].updateTime = 1.0f / 16.0f;
        nebulaArray[i].runningTime = 0.0f;
    }

    // finish line
    float finishLine{nebulaArray[sizeOfNebulaArray - 1].pos.x};

    // nebula X velocity (pixels/second)
    int nebVel{-200};

    // scarfy variables
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData{
        {0.0f, 0.0f, scarfy.width / 6.0f, scarfy.height / 1.0f},
        {windowWidth / 2 - scarfyData.rec.width / 2, windowHeight - scarfyData.rec.height - 10},
        0,
        1.0f / 8.0f,
        0.0f};

    // is the rectanlge in the air?
    bool isInAir{};

    // jump velocity (pixels/second)
    const int jumpVel{-600};

    int velocity{0};

    bool collision{};
    bool gameEnded{};

    while (!WindowShouldClose())
    {
        // delta time (time since last frame)
        const float dT{GetFrameTime()};

        // start drawing
        BeginDrawing();
        ClearBackground(WHITE);

        // background far
        bgFarX -= 20 * dT;
        if (bgFarX <= -backgroundFar.width * 2)
        {
            bgFarX = 0.0f;
        }
        DrawTextureEx(backgroundFar, {bgFarX, 0.0f}, 0.0f, 2.0f, WHITE);
        DrawTextureEx(backgroundFar, {bgFarX + backgroundFar.width * 2, 0.0f}, 0.0f, 2.0f, WHITE);

        // background mid
        bgMidX -= 40 * dT;
        if (bgMidX <= -backgroundMid.width * 2)
        {
            bgMidX = 0.0f;
        }
        DrawTextureEx(backgroundMid, {bgMidX, 0.0f}, 0.0f, 2.0f, WHITE);
        DrawTextureEx(backgroundMid, {bgMidX + backgroundMid.width * 2, 0.0f}, 0.0f, 2.0f, WHITE);

        // background close
        bgCloseX -= 80 * dT;
        if (bgCloseX <= -backgroundClose.width * 2)
        {
            bgCloseX = 0.0f;
        }
        DrawTextureEx(backgroundClose, {bgCloseX, 0.0f}, 0.0f, 2.0f, WHITE);
        DrawTextureEx(backgroundClose, {bgCloseX + backgroundClose.width * 2, 0.0f}, 0.0f, 2.0f, WHITE);

        DrawFPS(0, 0);

        // ground check
        isInAir = isInAirCheck(scarfyData, windowHeight);
        if (isInAir)
        {
            // air
            velocity += gravity * dT;
            isInAir = true;
        }
        else
        { // ground
            velocity = 0;
            isInAir = false;
        }

        // jump check
        if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP)) && !isInAir && !gameEnded)
        {
            velocity += jumpVel;
        }

        // update all nebula pos
        for (int i = 0; i < sizeOfNebulaArray; i++)
        {
            nebulaArray[i].pos.x += nebVel * dT;
        }

        // update finish line pos
        finishLine += nebVel * dT;

        // update scarfy pos
        scarfyData.pos.y += velocity * dT;

        // update scarfy animation frame
        if (!isInAir)
        {
            scarfyData = UpdateAnimData(scarfyData, dT, 5);
        }

        // update nebulas animation frame
        for (int i = 0; i < sizeOfNebulaArray; i++)
        {
            nebulaArray[i] = UpdateAnimData(nebulaArray[i], dT, 7);
        }

        for (AnimData nebula : nebulaArray)
        {
            float pad{50};
            Rectangle nebHitBox{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2 * pad,
                nebula.rec.height - 2 * pad};

            Rectangle scarfyHitBox{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height};

            if (CheckCollisionRecs(nebHitBox, scarfyHitBox))
            {
                collision = true;
            }
        }

        if (collision)
        {   
            gameEnded = true;
            DrawText("Game Over!", windowWidth / 2 - 150, windowHeight / 2 - 60, 60, RED);
            DrawText("Press R to restart", windowWidth / 2 - 150, windowHeight / 2 + 10 , 35, RED);
        }
        else if (scarfyData.pos.x >= finishLine && !isInAir)
        {   
            gameEnded = true;
            DrawText("You Win!", windowWidth / 2 - 130, windowHeight / 2 - 50, 60, GREEN);
            DrawText("Press R to restart", windowWidth / 2 - 150, windowHeight / 2 + 10 , 35, GREEN);
        }
        else
        {
            // draw nebulas
            for (int i = 0; i < sizeOfNebulaArray-1; i++)
            {
                DrawTextureRec(nebula, nebulaArray[i].rec, nebulaArray[i].pos, WHITE);
            }
            //last nebula
            DrawTextureRec(nebula, nebulaArray[sizeOfNebulaArray-1].rec, nebulaArray[sizeOfNebulaArray-1].pos, RED);

            // draw scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }

        //reset game
        if(IsKeyPressed(KEY_R) && gameEnded)
        {
            scarfyData.pos = {windowWidth / 2 - scarfyData.rec.width / 2, windowHeight - scarfyData.rec.height};
            for (int i = 0; i < sizeOfNebulaArray; i++)
            {
                nebulaArray[i].pos = {windowWidth + i * 500.0f + GetRandomValue(0,300), windowHeight - nebula.height / 8.0f};
            }
            finishLine = nebulaArray[sizeOfNebulaArray - 1].pos.x;
            collision = false;
            gameEnded = false;
        }

        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(backgroundFar);
    UnloadTexture(backgroundMid);
    UnloadTexture(backgroundClose);
    CloseWindow();
}