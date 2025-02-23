#include <stdio.h>
#define RCAMERA_IMPLEMENTATION
#define RL_CULL_DISTANCE_FAR 1000
#define RL_CULL_DISTANCE_NEAR 0.01 
#define RCAMERA_IMPLEMENTATION

#include <raylib.h>
#include "rcamera.h"
#include <raymath.h>
#include <time.h>
#include <stdlib.h>

#define CUBE_SIZE 0.5

int main(int argc, char **argv){
	InitWindow(1600, 900, "WaveFunctionColapse Test");
	DisableCursor();

	Camera3D cam3d = {0};
	cam3d.position = (Vector3){CUBE_SIZE * 2,CUBE_SIZE * 2, CUBE_SIZE * 2};
	cam3d.target = (Vector3){0, 0, 0};
	cam3d.up = (Vector3){0, 1, 0};
	cam3d.fovy = 60.0;
	cam3d.projection = CAMERA_PERSPECTIVE;

	SetTargetFPS(60);

	while(!WindowShouldClose()){
		BeginDrawing();
		ClearBackground(BLACK);
		BeginMode3D(cam3d);
		UpdateCamera(&cam3d, CAMERA_FREE);
		TraceLog(LOG_INFO, "(%f, %f, %f)", cam3d.position.x, cam3d.position.y, cam3d.position.z);

		for(int i = 0; i < 10; i++){
			DrawCubeV((Vector3){0, CUBE_SIZE * i, 0}, (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}, RED);
		}
		
		DrawPlane((Vector3){0,0,0}, (Vector2){10000, 10000}, BLUE);
		DrawGrid(10000, CUBE_SIZE * 4);
		EndMode3D();

		DrawFPS(10, 10);

		EndDrawing();
	}
	return 0;
}
