#pragma once
#include "tools/Vector2.h"

#include <string>

namespace FL = FlatEngine;


namespace FlatEngine
{
    namespace GameView
    {
        extern float GAME_VIEWPORT_WIDTH;
        extern float GAME_VIEWPORT_HEIGHT;
        extern float xGameCenter;
        extern float yGameCenter;
        extern FL::Vector2 gameViewCenter;
        extern float gameViewGridStep;

       	extern void RenderGameView(bool& b_show, bool b_inRuntime = false);
        extern void DrawLineInGame(FL::Vector2 startingPoint, FL::Vector2 endingPoint, std::string color = "white", float thickness = 1.0f);
        extern FL::Vector2 ConvertWorldToScreen(FL::Vector2 positionInWorld);
        extern FL::Vector2 ConvertScreenToWorld(FL::Vector2 positionOnScreen);
        extern FL::Vector2 GetMousePosWorld();
        extern FL::Vector2 GetMousePosScreen();
    }
}