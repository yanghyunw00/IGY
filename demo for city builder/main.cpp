#include <raylib.h>
#include <iostream>
#include "GridMap.h"
#include "Mode7.h"
#include "CustomCell.h"
#include "Human.h"
#include "Road.h"

int main()
{
    const int screenWidth = 2800;
    const int screenHeight = 1400;
    InitWindow(screenWidth, screenHeight, "I.G.Y");
    SetTargetFPS(540);
    srand(time(NULL));

    GridMap map;
    MouseInput mouse;
    Human human({ 5, 5 });
	Road road;

    int index = 0; 
    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            map.ResetPerlinTexture();
            map.ResetIsoGridMapArray();
            map.InitializeIsoView();
            map.IsometricExportMap(1);
            map.IsometricExportMap(2);
            map.IsometricExportMap(3);
            map.IsometricExportMap(4);
            map.InitializePathFinding(); 
            map.ClearPath(); 
        }

        if (IsKeyPressed(KEY_RIGHT)) index++;
        if (IsKeyPressed(KEY_LEFT)) index--;
        int currentView = (index % 4) + 1;

        if (IsKeyPressed(KEY_R))
        {
            map.ClearPath();  
            int sx, sy, ex, ey;
            std::cout << "start pos (x y): ";
            std::cin >> sx >> sy;
            std::cout << "end pos (x y): ";
            std::cin >> ex >> ey;

            if (sx >= 0 && sx < GRID_SIZE_X && sy >= 0 && sy < GRID_SIZE_Y &&
                ex >= 0 && ex < GRID_SIZE_X && ey >= 0 && ey < GRID_SIZE_Y)
            {
                std::cout << "finding.. (" << sx << ", " << sy << ") -> (" << ex << ", " << ey << ")" << std::endl;
                map.FindPath(sx, sy, ex, ey);
                human.FollowPath(map.GetCurrentPath());
            }
            else
            {
                std::cout << " set the valid range: (0-" << GRID_SIZE_X - 1 << ", 0-" << GRID_SIZE_Y - 1 << ")" << std::endl;
            }
        }

		//-------------Road--------------------
		if (IsKeyPressed(KEY_W))
		{
			int rx, ry;
			std::cout << "Put Single road location (x y) : ";
			std::cin >> rx >> ry;

			if (rx >= 0 && rx < GRID_SIZE_X && ry >= 0 && ry < GRID_SIZE_Y)
			{
				std::cout << "Put road on.. (" << rx << ", " << ry << ")" << std::endl;
				road.addRoad(rx, ry);
			}
			else if (road.hasRoad(rx, ry) == true)
			{
				std::cout << "Already built in there" << std::endl;
			}
			else
			{
				std::cout << " set the valid range: (0-" << GRID_SIZE_X - 1 << ", 0-" << GRID_SIZE_Y - 1 << ")" << std::endl;
			}
		}

		if (IsKeyPressed(KEY_E))
		{
			int sx, sy, ex, ey;
			std::cout << "Start Pos(x y) : ";
			std::cin >> sx >> sy;
			std::cout << "End Pos(x y) : ";
			std::cin >> ex >> ey;
			std::cout << '\n';

			if (sx >= 0 && sx < GRID_SIZE_X && sy >= 0 && sy < GRID_SIZE_Y &&
				ex >= 0 && ex < GRID_SIZE_X && ey >= 0 && ey < GRID_SIZE_Y)
			{
				road.buildRoadPath(map, sx, sy, ex, ey);
			}
			else
			{
				std::cout << "In valid range" << std::endl;
			}
		}

		if (IsKeyPressed(KEY_D))
		{
			road.removeAllRoad();
		}

        //Draw Human
		human.Update(GetFrameTime(), &map);

        if (IsKeyPressed(KEY_C))
        {
            map.ClearPath();
            std::cout << "path clear" << std::endl;
        }

		if(IsKeyPressed(KEY_F))
		{
			human.ToggleDebug();
			std::cout << "Debug: " << (human.IsDebugMode() ? "ON" : "OFF") << std::endl;
		}



        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawFPS(1800, 10);


        map.IsometricDrawMap(index);
		road.drawRoads(map, currentView);
        human.Draw(currentView, &map);

        DrawText(TextFormat("View: %d", (index % 4) + 1), 10, 10, 20, BLACK);
        DrawText("P: Find Path | C: Clear Path | F: Debug | SPACE: New Map | Arrow Keys: Rotate View", 10, 40, 20, DARKGRAY);
		DrawText("W: Place single Road | E: Place roads | D: Clear All Roads", 10, 70, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
